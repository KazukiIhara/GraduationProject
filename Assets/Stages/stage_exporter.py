bl_info = {
    "name": "卒業制作_ステージエディタ",
    "author": "Game Dev",
    "version": (1, 2),
    "blender": (4, 4, 0),
    "location": "View3D > Sidebar (N) > Stage Tool",
    "description": "Export stage JSON and spawn models from Models folder",
    "category": "Import-Export",
}

import bpy
import json
import math
import os
from bpy.props import StringProperty, BoolProperty
from bpy_extras.io_utils import ExportHelper

# ==========================================
# 1. 共通ユーティリティ（座標変換など）
# ==========================================

def convert_location_to_yup_lh(loc):
    return [round(loc.x, 4), round(loc.z, 4), round(loc.y, 4)]

def convert_rotation_to_yup_lh(rot_euler):
    rx = -rot_euler.x
    ry = -rot_euler.z
    rz = -rot_euler.y
    return [round(rx, 4), round(ry, 4), round(rz, 4)]

def convert_scale_to_yup(scale):
    return [round(scale.x, 4), round(scale.z, 4), round(scale.y, 4)]

def clean_object_name(name):
    if "." in name:
        base, ext = name.rsplit(".", 1)
        if ext.isdigit():
            return base
    return name


# ==========================================
# 2. モデル配置機能（Modelsフォルダからの読み込み）
# ==========================================

class STAGE_OT_spawn_model(bpy.types.Operator):
    """Modelsフォルダから指定したモデルをシーンに配置する"""
    bl_idname = "stage.spawn_model"
    bl_label = "Spawn Model"

    # フォルダのサブパス（例: "environment/tree"）とモデル名（例: "tree_01"）を指定
    sub_dir: StringProperty(default="environment/tree")
    model_name: StringProperty(default="tree_01")
    category: StringProperty(default="Environment") # JSON出力時のtype用

    def execute(self, context):
        if not bpy.data.is_saved:
            self.report({'ERROR'}, "先にBlenderファイルを保存してください（相対パス解決のため）")
            return {'CANCELLED'}
        
        # 現在地: GraduationProject/Assets/Stages/
        blend_dir = os.path.dirname(bpy.data.filepath)
        
        # 1つ上の階層(Assets)に戻り、Modelsフォルダへのパスを構築
        # 例: Assets/Models/environment/tree/tree_01/tree_01.gltf
        models_base_dir = os.path.abspath(os.path.join(blend_dir, "..", "Models"))

        # パスの組み立て（モデル名と同名のフォルダがある構成、または直下の構成両方に対応）
        # パターンA: Models/sub_dir/model_name/model_name.gltf
        model_file_path = os.path.join(models_base_dir, self.sub_dir, self.model_name, f"{self.model_name}.gltf")
        
        # パターンBで見つからない場合、Models/sub_dir/model_name.gltf も探索
        if not os.path.exists(model_file_path):
            model_file_path = os.path.join(models_base_dir, self.sub_dir, f"{self.model_name}.gltf")

        # .glb も念のため探索
        if not os.path.exists(model_file_path):
            model_file_path = os.path.join(models_base_dir, self.sub_dir, self.model_name, f"{self.model_name}.glb")
        if not os.path.exists(model_file_path):
            model_file_path = os.path.join(models_base_dir, self.sub_dir, f"{self.model_name}.glb")
            
        if not os.path.exists(model_file_path):
            self.report({'ERROR'}, f"モデルファイルが見つかりません:\n{model_file_path}")
            return {'CANCELLED'}

        # Blenderの標準glTFインポーターを実行
        bpy.ops.import_scene.gltf(filepath=model_file_path)

        # インポートされたオブジェクトを指定のコレクションに割り当てる
        imported_objects = context.selected_objects
        if imported_objects:
            col_name = self.category
            if col_name not in bpy.data.collections:
                new_col = bpy.data.collections.new(col_name)
                context.scene.collection.children.link(new_col)
            else:
                new_col = bpy.data.collections[col_name]

            for obj in imported_objects:
                for old_col in obj.users_collection:
                    old_col.objects.unlink(obj)
                new_col.objects.link(obj)
                obj.name = self.model_name

        self.report({'INFO'}, f"配置しました: {self.model_name}")
        return {'FINISHED'}


# ==========================================
# 3. JSONエクスポート機能
# ==========================================

class EXPORT_OT_stage_json(bpy.types.Operator, ExportHelper):
    """ステージ配置データをJSONとして出力"""
    bl_idname = "export_scene.stage_json"
    bl_label = "Export Stage JSON"
    filename_ext = ".json"

    filter_glob: StringProperty(
        default="*.json",
        options={'HIDDEN'},
        maxlen=255,
    )

    only_visible: BoolProperty(
        name="表示中のオブジェクトのみ出力",
        default=True,
    )

    def execute(self, context):
        stage_objects = []

        for obj in context.scene.objects:
            if obj.type != 'MESH':
                continue

            if self.only_visible and not obj.visible_get():
                continue

            category_type = "Default"
            if obj.users_collection:
                col_name = obj.users_collection[0].name
                if col_name != "Scene Collection":
                    category_type = col_name

            model_name = clean_object_name(obj.name)
            matrix_world = obj.matrix_world
            loc = matrix_world.to_translation()
            rot = matrix_world.to_euler('XYZ')
            scale = matrix_world.to_scale()

            obj_data = {
                "type": category_type,
                "name": model_name,
                "position": convert_location_to_yup_lh(loc),
                "rotation": convert_rotation_to_yup_lh(rot),
                "scale": convert_scale_to_yup(scale),
            }
            stage_objects.append(obj_data)

        output_data = {
            "stage_name": os.path.splitext(os.path.basename(self.filepath))[0],
            "object_count": len(stage_objects),
            "objects": stage_objects
        }

        with open(self.filepath, 'w', encoding='utf-8') as f:
            json.dump(output_data, f, indent=4, ensure_ascii=False)

        self.report({'INFO'}, f"Stage JSON exported: {len(stage_objects)} objects.")
        return {'FINISHED'}


# ==========================================
# 4. サイドバーUIパネルの定義（テスト3モデル配置）
# ==========================================

class VIEW3D_PT_stage_manager_panel(bpy.types.Panel):
    bl_label = "Stage Manager"
    bl_idname = "VIEW3D_PT_stage_manager_panel"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = 'Stage Tool'

    def draw(self, context):
        layout = self.layout

        layout.label(text="オブジェクト配置 (テスト):")

        ### 環境モデル

        # 地面配置ボタン
        row = layout.row()
        op = row.operator(STAGE_OT_spawn_model.bl_idname, text="地面を配置")
        op.sub_dir = "environment/ground"
        op.model_name = "ground"
        op.category = "Environment"
        
        # 木01配置ボタン
        row = layout.row()
        op = row.operator(STAGE_OT_spawn_model.bl_idname, text="木01を配置")
        op.sub_dir = "environment/tree_01"
        op.model_name = "tree_01"
        op.category = "Environment"

        # 木02配置ボタン
        row = layout.row()
        op = row.operator(STAGE_OT_spawn_model.bl_idname, text="木02を配置")
        op.sub_dir = "environment/tree_02"
        op.model_name = "tree_02"
        op.category = "Environment"

        # 岩01配置ボタン
        row = layout.row()
        op = row.operator(STAGE_OT_spawn_model.bl_idname, text="岩01を配置")
        op.sub_dir = "environment/rock_01"
        op.model_name = "rock_01"
        op.category = "Environment"

        # 草01配置ボタン
        row = layout.row()
        op = row.operator(STAGE_OT_spawn_model.bl_idname, text="草01を配置")
        op.sub_dir = "environment/grass_01"
        op.model_name = "grass_01"
        op.category = "Environment"

        ### 家具モデル

        # 中華レンジ配置ボタン
        row = layout.row()
        op = row.operator(STAGE_OT_spawn_model.bl_idname, text="中華レンジを配置")
        op.sub_dir = "furniture/chineseRange"
        op.model_name = "chineseRange"
        op.category = "Furniture"

        layout.separator()
        layout.label(text="データ出力:")
        layout.operator(EXPORT_OT_stage_json.bl_idname, icon='EXPORT', text="JSONをエクスポート")


def menu_func_export(self, context):
    self.layout.operator(EXPORT_OT_stage_json.bl_idname, text="Stage Layout (.json)")


classes = (
    STAGE_OT_spawn_model,
    EXPORT_OT_stage_json,
    VIEW3D_PT_stage_manager_panel,
)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

def unregister():
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()