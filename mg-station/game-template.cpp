#include "acex_draw_ex\include\acex_draw_ex.h"

#include "game-template.h"//ゲームクラス宣言ヘッダ

std::pair<int, game_util::GameCommand> game_main::GameClass::main(const game_util::COMMON_RESOURCE &resoure)
{
	using namespace acex::draw;
	using namespace acex::draw::ex;

	auto draw = resoure.idraw;
	auto camera2D = resoure.camera2D;
	auto screent = resoure.iscreent;
	auto ioimg = resoure.ioImg;
	auto& timeKeeper = *resoure.keeper;
	auto& inputmng = *resoure.inputMng;
	auto ScreenW = resoure.screenW, ScreenH = resoure.screenH;
	bool isFirstScene;

	//--リソース定義　始--
	//ここにコードを追加
	//--リソース定義　終--
	game_util::DrawSyncGuarder dsg(draw);//リソースの定義の後に置く

BEGIN:
	{
		//--ゲーム開始時の初期化　始--
		timeKeeper.reset();
		isFirstScene = true;
		//--ゲーム開始時の初期化　終--

		while (timeKeeper.keepTime()) {
			acs::vector::i2d screenSize;
			AppBase::GetScreenSize(screenSize);
			//スクリーンサイズ変更時　処理
			if (AppBase::ScreenCheckResized()) {
				if (false == draw->ResizeScreenTarget({ (acs::uint)screenSize.x ,(acs::uint)screenSize.y }))throw(std::exception("screen resize err"));
			}

			//--ゲームのメイン処理　始--
			{
				inputmng.updateInputState();

				/*
				if (inputmng.getKeyHoldingtick(KEY_CODE) == 1) {
					ボタンが押された瞬間の処理
				}
				if(inputmng.getKeyHoldingtick(KEY_CODE) > 0){
					ボタンが押されている間ずっとの処理
				}
				if (inputmng.getKeyHoldingtick(KEY_CODE) == -1) {
				ボタンが離された瞬間の処理
				}
				*/

				if (inputmng.getKeyHoldingtick(VK_ESCAPE) == 1) {
					//escで戻る
					return std::pair<int, game_util::GameCommand>(0, game_util::GameCommand::EndApp);
				}
			}
			//--ゲームのメイン処理　終--


			//--描画関連処理　始--
			//表示
			if (isFirstScene) isFirstScene = false;
			else draw->Present(0);

			//描画データ更新
			{
				Updater updater(draw);
				//データ更新
			}

			//描画
			{
				Drawer context(draw);
				//float backGnd[] = { 0,0,0,0 };
				//context->SetTargets(1, &screent, nullptr);
				//context->ClearTarget(0, backGnd);
				//オブジェクト描画
			}
			//--描画関連処理　終--
		}
		return std::pair<int, game_util::GameCommand>(0, game_util::GameCommand::EndApp);
	}
}
