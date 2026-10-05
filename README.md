# Lazieal Game Template

LaziealRuntime、DirectX 12、C++20を使用するゲームのひな型です。
生成されるVisual Studioワークスペース名、プロジェクト名、実行ファイル名には
リポジトリのフォルダ名が自動的に使われます。

## 新しいゲームを作る

GitHubの「Use this template」から新しいリポジトリを作成し、
依存リポジトリを含めてクローンします。

```powershell
git clone --recursive <repository-url>
cd <repository-name>
Project\Premake.bat
```

生成された`Project/<repository-name>.slnx`をVisual Studioで開き、
x64構成をビルドします。

再帰オプションを付けずにクローンした場合は、次のコマンドで依存リポジトリを取得できます。

```powershell
git submodule update --init --recursive
```

## ディレクトリ構成

- `Project/Src`: ゲーム固有のソースコード
- `Assets`: モデル、テクスチャ、サウンド、JSONなどのゲーム用データ
- `Project/RuntimeSetting.ini`: ウィンドウと入力などの起動設定
- `Dependencies/LaziealRuntime`: ゲームが使用するエンジンの固定リビジョン

ゲーム固有のコードとアセットは`Dependencies`の外に配置してください。
エンジンを更新するときは、LaziealRuntime submoduleで動作確認済みのタグまたは
コミットをチェックアウトし、変更されたsubmoduleの参照をゲーム側へコミットします。

## 初期シーン

テンプレートには、カメラ操作と回転する3Dボックスを確認できる最小構成の
`MainScene`が含まれています。ゲーム制作を始めるときは、
`Project/Src/Scene`へシーンを追加してください。

## Joy-Con

既定では左右のJoy-Conを別々のコントローラーとして取得します。

```cpp
if (const auto left = LGF::JoyCon::Left()) {
    const LGF::Vector3 gyro = left->Gyroscope();
}

if (const auto right = LGF::JoyCon::Right()) {
    const LGF::Vector3 gyro = right->Gyroscope();
}
```

認識方法は`Project/RuntimeSetting.ini`の`[Input.Gamepad]`から変更できます。
