#include "acs\include\def.h"
#include "AppBase.h"
#include "acex_draw_ex\include\acex_draw_ex.h"

#include "block_blake.h"

#define ThrowIfFalse(x) if(!x)throw(#x);
#define LOOP(x,y) for(size_t x = 0;x < y;++x)
namespace {
	using acs::vector::i2d;
	using acs::vector::f2d;
	using acs::vector::f3d;
	using namespace acex::draw::ex;
	sprite::SPRITE_STATE ss_block = { true,{ 0,0,0 },{ 32,16 },{ 0,0,0 },{ 0,0,1,1 } };
	sprite::SPRITE_STATE ss_bar = { true,{ 0,-280,0 },{ 128,16 },{ 0,0,0 },{ 0,0,1,1 } };
	sprite::SPRITE_STATE ss_boll = { false,{ -400, -200,0 },{ 8,8 },{ 0,0,0 },{ 0,0,1,1 } };
	const float balloffsx = 0.1f;
	const float glabity = 0;
	const float boundoffs = 0.001f;
	const int balls = 1;
	const float ball_first_speed = 7;
	const float ball_max_speed = 32;

	const int blockx = 16;
	const int blocky = 16;
	const int blocks = blockx *blocky;
	const float defbarspeed = 14;
	const float exspeedoffs = 3;
}

int game_main::BlockBrake::main(const game_util::COMMON_RESOURCE& resoure) {
	using namespace acex::draw;

	auto draw = resoure.idraw;
	auto camera2D = resoure.camera2D;
	auto screent = resoure.iscreent;
	auto& timeKeeper = *resoure.keeper;
	auto& inputmng = *resoure.inputMng;
	auto ScreenW = resoure.screenW, ScreenH = resoure.screenH;
	bool isFirstScene;

	//--リソース定義　始--
	acs::SIACS<IRenderResource> res;
	COLOR color[] = { { 255,255,255,255 } };
	CreateIRenderResourceM(draw, 1, 1, color, &res);
	sprite::SimpleSprite bar(draw, 1, camera2D, res);
	sprite::SimpleSprite ball(draw, balls, camera2D, res);
	f2d vball[balls];//ボールの移動方向(単位ベクトル)
	float sball[balls];//ボールの移動の速さ
	sprite::SimpleSprite block(draw, blocks, camera2D, res);
	std::vector<bool> ballRemoveReserved(blocks);
	//--リソース定義　終--
	game_util::DrawSyncGuarder dsg(draw);//リソースの定義の後に置く

BEGIN:

	//--ゲーム開始時の初期化　始--
	timeKeeper.reset();
	isFirstScene = true;

	bool Started = false;

	bar[0] = ss_bar;
	LOOP(i,balls) {
		auto& stats = ball[i];
		stats = ss_boll;
		vball[i] = { 0,1 };
		sball[i] = ball_first_speed;
		vball[i] = acs::vector::normalize(vball[i]);
		vball[i] *= sball[i];
	}
	LOOP(j, blocky) {
		LOOP(i, blockx) {
			auto& stats = block[blockx * j + i];
			stats = ss_block;
			stats.pos = f3d((-ss_block.size.x *blockx / 2) + ss_block.size.x * (int)i, ((blocky * ss_block.size.y) / 2) + 100 - ss_block.size.y * (int)j, 0);
			ballRemoveReserved[blockx * j + i] = false;
		}
	}
	//--ゲーム開始時の初期化　終--

	while (timeKeeper.keepTime()) {

		acs::vector::i2d screenSize;
		AppBase::GetScreenSize(screenSize);
		//スクリーンサイズ変更時　処理
		if (AppBase::ScreenCheckResized()) {
			ThrowIfFalse(draw->ResizeScreenTarget({ (acs::uint)screenSize.x ,(acs::uint)screenSize.y }));
		}

		//--ゲームのメイン処理　始--
		{
			auto& stat_bar = bar[0];
			float speedoffs = 1;
			inputmng.updateInputState();

			if (inputmng.getKeyHoldingtick(VK_SPACE) == 1) {
				if (!Started) {
					Started = true;
					for (int i = 0;i < balls;++i) {
						auto& stats = ball[i];
						stats.flag = true;
						stats.pos = stat_bar.pos + f3d(balloffsx * (i - (balls / 2)), 0, 0);
					}
				}
			}

			if (inputmng.getKeyHoldingtick(VK_UP) > 0) {
				speedoffs = exspeedoffs;
			}
			if (inputmng.getKeyHoldingtick(VK_LEFT) > 0) {
				stat_bar.pos.x -= defbarspeed * speedoffs;
			}
			if (inputmng.getKeyHoldingtick(VK_RIGHT) > 0) {
				stat_bar.pos.x += defbarspeed * speedoffs;
			}

			if (inputmng.getMouseHoldingtick(0) > 0) {
				stat_bar.pos.x = inputmng.getMousePos({ ScreenW,ScreenH }, screenSize).x;
			}
			if (inputmng.getKeyHoldingtick(VK_ESCAPE) == 1) {
				return 0;
			}

			//バーが画面外に出ないようにする
			if (stat_bar.pos.x < (stat_bar.size.x - ScreenW) / 2) {
				stat_bar.pos.x = (stat_bar.size.x - ScreenW) / 2;
			}
			if (stat_bar.pos.x >(ScreenW - stat_bar.size.x) / 2) {
				stat_bar.pos.x = (ScreenW - stat_bar.size.x) / 2;
			}

			LOOP(b, balls) {
				auto& stat_ball = ball[b];
				if (stat_ball.flag) {
					float fD = sball[b];
					while (fD > 0) {
						int i_near_block = -1;

						float dx = vball[b].x * std::min(fD, stat_ball.size.x / 2);
						float dy = vball[b].y * std::min(fD, stat_ball.size.y / 2);

						//ブロックとヒットチェック
						bool hitlr = false, hittb = false;
						float swap_reserve;
						for (size_t i = 0; i < blocks; i++)
						{
							auto& stat_block = block[i];
							if (stat_block.flag) {
								float t_hit_x0 = ((stat_block.pos.x - stat_block.size.x / 2) - (stat_ball.pos.x + stat_ball.size.x / 2)) / dx;
								float t_hit_x1 = ((stat_block.pos.x + stat_block.size.x / 2) - (stat_ball.pos.x - stat_ball.size.x / 2)) / dx;
								if (t_hit_x0 > t_hit_x1) {
									swap_reserve = t_hit_x0;
									t_hit_x0 = t_hit_x1;
									t_hit_x1 = swap_reserve;
								}

								float t_hit_y0 = ((stat_block.pos.y - stat_block.size.y / 2) - (stat_ball.pos.y + stat_ball.size.y / 2)) / dy;
								float t_hit_y1 = ((stat_block.pos.y + stat_block.size.y / 2) - (stat_ball.pos.y - stat_ball.size.y / 2)) / dy;
								if (t_hit_y0 > t_hit_y1) {
									swap_reserve = t_hit_y0;
									t_hit_y0 = t_hit_y1;
									t_hit_y1 = swap_reserve;
								}

								if (0 <= t_hit_y0 && t_hit_y0 <= 1 && std::abs(stat_block.pos.x - stat_ball.pos.x - dx * t_hit_y0) <= (stat_block.size.x + stat_ball.size.x) / 2.f) {
									ballRemoveReserved[i] = true;
									hittb = true;
								}
								if (0 <= t_hit_x0 && t_hit_x0 <= 1 && std::abs(stat_block.pos.y - stat_ball.pos.y - dy * t_hit_x0) <= (stat_block.size.y + stat_ball.size.y) / 2.f) {
									ballRemoveReserved[i] = true;
									hitlr = true;
								}
							}
						}

						stat_ball.pos.x += dx;
						stat_ball.pos.y += dy;
						fD -= stat_ball.size.x / 2;

						if (hitlr) {
							vball[b].x = -vball[b].x;
						}
						if (hittb) {
							vball[b].y = -vball[b].y;
						}

						//バーに当たった時
						f2d dbb = stat_ball.pos.xy() - stat_bar.pos.xy();
						if ((std::abs(dbb.x) <= (stat_ball.size.x + stat_bar.size.x) / 2) && (std::abs(dbb.y) <= (stat_ball.size.y + stat_bar.size.y) / 2)) {
							if (vball[b].y < 0)vball[b].y = -vball[b].y;
							vball[b].x += dbb.x / stat_bar.size.x;
							sball[b] += boundoffs;
						}

						//壁に当たった時跳ね返る
						if (stat_ball.pos.x <= (stat_ball.size.x - ScreenW) / 2) {
							if (vball[b].x < 0)vball[b].x = -vball[b].x;
						}
						if (stat_ball.pos.x >= (ScreenW - stat_ball.size.x) / 2) {
							if (vball[b].x > 0)vball[b].x = -vball[b].x;
						}
						if (stat_ball.pos.y >= (ScreenH - stat_ball.size.y) / 2) {
							if (vball[b].y > 0)vball[b].y = -vball[b].y;
						}

						//ボールが下に落下するとき
						if (stat_ball.pos.y <= -(ScreenH - stat_ball.size.y) / 2) {
							//ボールが落下しないとき
#ifdef BALL_NO_FALL
							if (vball[b].y < 0)vball[b].y = -vball[b].y;
#else
							stat_ball.flag = false;
							continue;
#endif
						}

						vball[b] = acs::vector::normalize(vball[b]);
						if (sball[b] > ball_max_speed)sball[b] = ball_max_speed;
					}
				}
			}

			//削除予約されたブロックを消す
			for (size_t i = 0; i < blocks; i++)
			{
				auto& stat_block = block[i];
				if (stat_block.flag && ballRemoveReserved[i]) {
					stat_block.flag = false;
				}

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
			block.Update(updater);
			ball.Update(updater);
			bar.Update(updater);
		}

		//描画
		{
			Drawer drawer(draw);
			float backGnd[] = { 0,0,0,0 };
			drawer->SetTargets(1, &screent, nullptr);
			drawer->ClearTarget(0, backGnd);
			block.Draw(drawer);
			ball.Draw(drawer);
			bar.Draw(drawer);
		}
		//--描画関連処理　終--
	}
	return 0;
}