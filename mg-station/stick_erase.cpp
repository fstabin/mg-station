#include <vector>
#include <tuple>

#include "acex_draw_ex\include\sprite.h"
#include "acex_draw_ex\include\primitive.h"

#include "AppBase.h"

#include "stick_erase.h"

#define ThrowIfFalse(x) if(!x)throw(#x);
namespace {
	using acs::vector::i2d;
	using acs::vector::f2d;
	using acs::vector::f3d;

	using namespace acex::draw;
	using namespace acex::draw::ex;

	class GameManager {
		size_t mLineAmount;//列の個数 L
		size_t mStickSum;//棒の合計 M
		size_t mStickErasedSum;//消した棒の合計 M
		sprite::SimpleSprite mSprite; //サイズ(M)
		std::vector<size_t>mSpriteIndex;//l列目の先頭の番号 サイズ(L)
		std::vector<bool>mStickIsErased;//m個目が消されているかどうか サイズ(M)
	public:
		static size_t calcStickSum(size_t arraySize, const int stickAmounts[]) {
			int stickSum = 0;
			for (size_t i = 0; i < arraySize; i++)
			{
				stickSum += stickAmounts[i];
			}
			return stickSum;
		}

		GameManager(size_t arraySize, const int stickAmounts[],
			IDraw* draw, ICamPro* a_op_cam, IRenderResource* a_op_resource)
			:mLineAmount(arraySize),
			mStickSum(calcStickSum(arraySize, stickAmounts)),
			mSprite(draw, mStickSum, a_op_cam , a_op_resource),
			mSpriteIndex(arraySize + 1),
			mStickIsErased(mStickSum){

			size_t index = mSpriteIndex[0] = 0;
			for (size_t i = 0; i < mLineAmount; i++)
			{
				index = mSpriteIndex[i + 1] = index + stickAmounts[i];
			}
		}

		void init() {
			size_t index = 0;
			mStickErasedSum = 0;
			f3d pos = { 0, 64.f * mLineAmount / 2.f,0 };
			for (size_t i = 0; i < mLineAmount; i++)
			{
				int sm = mSpriteIndex[i + 1] - mSpriteIndex[i];
				pos.x = -64. * sm / 2.f;
				for (size_t j = 0; j < sm; j++)
				{
					auto& state = mSprite[index];
					state.flag = true;
					state.pos = pos;
					state.size = { 32,64 };
					state.texstate = { 0,0,0.5,1 };
					pos.x += 64;
					mStickIsErased[index] = false;
					++index;
				}
				pos.y -= 64;
			}
		}

		bool sticksAreInLine(int line, int from, int to)const {
			auto l = mSpriteIndex[line - 1];
			if (from > to)return false;
			if(l + to > mSpriteIndex[line])return false;
			for (size_t i = from - 1; i < to; i++)
			{
				if (mStickIsErased[l + i] == true)return  false;
			}
			return true;
		}

		bool eraseStick(int line, int from, int to) {
			if (sticksAreInLine(line,from,to) == false)return false;//棒が並んでないとき除外
			auto l = mSpriteIndex[line - 1];
			for (size_t i = from - 1; i < to; i++)
			{
				mSprite[l + i].texstate = {0.5,0,0.5,1};
				mStickIsErased[l + i] = true;
			}
			mStickErasedSum += (to - from + 1);
			return true;
		}

	private:
		std::tuple<bool, int, int,int> getLineFromTo(f2d p1, f2d p2) const {
			float cy = mSprite[0].pos.y + 32;
			if (p1.y > cy)return{false,0,0,0};
			if (p2.y > cy)return{ false,0,0,0 };
			int line1 = -(p1.y - cy) / 64 + 1;
			int line2 = -(p2.y - cy) / 64 + 1;
			if (line1 != line2)return{ false,0,0,0 };
			if (line1 < 1 || mLineAmount < line1)return{ false,0,0,0 };
			float clx = mSprite[mSpriteIndex[line1 - 1]].pos.x;
			float crx = mSprite[mSpriteIndex[line1] - 1].pos.x;
			float lx = std::min(p1.x, p2.x);
			float rx = std::max(p1.x, p2.x);
			int from, to;
			if (lx > crx)return{ false,0,0,0 };
			else if (lx < clx)from = 1;
			else from = (lx - clx) / 64.f + 2;
			if (rx < clx)return{ false,0,0,0 };
			else if (rx > crx)to = mSpriteIndex[line1] - mSpriteIndex[line1 - 1];
			else to = (rx - clx) / 64.f + 1;
			return{ true,line1, from,to };
		}
	public:

		bool sticksAreOnLine(f2d p1, f2d p2) const {
			auto a = getLineFromTo(p1, p2);
			if (std::get<0>(a) == false)return false;
			return sticksAreInLine(std::get<1>(a), std::get<2>(a), std::get<3>(a));
		}

		bool eraseStick(f2d p1, f2d p2) {
			auto a = getLineFromTo(p1, p2);
			if (std::get<0>(a) == false)return false;
			return eraseStick(std::get<1>(a), std::get<2>(a), std::get<3>(a));
		}

		bool gameHasFinished() const{
			return mStickSum == mStickErasedSum;
		}

		void getStickStacks(std::vector<int>& outAmounts) const{
			outAmounts.clear();
			size_t i = 0;
			int stackSize = 0;//棒の塊の大きさ
			for (size_t e: mSpriteIndex)//列ごとに読み取る
			{
				for (; i < e; i++)
				{
					if (false == mStickIsErased[i])++stackSize;
					else if (stackSize > 0)outAmounts.push_back(stackSize);
				}
				//for (size_t i = 0; i < length; i++)
				//{
//
				//}
			}
		}

		void update(IUpdater* upd) {
			mSprite.Update(upd);
		}

		void draw(IDrawer* drawer) {
			mSprite.Draw(drawer);
		}
	};

	class IGamePlayer {
	public:
		virtual void onMyTurnBegin(const GameManager& gameMng) {

		}
		virtual bool getNewQuery(const game_util::InputManager& inputmng, acs::vector::f2d mousePos, acs::vector::f2d& pos) = 0;//入力
	};

	class  User :public IGamePlayer {
	public:
		virtual bool getNewQuery(const game_util::InputManager& inputmng, acs::vector::f2d mousePos,acs::vector::f2d& pos) {
			 if (inputmng.getMouseHoldingtick(0) >= 1) {
				 pos = mousePos;
				 return true;
			}
			 else return false;
		}
	};

	class ComputerPlayer :public IGamePlayer {

	public:
		virtual bool getNewQuery(const game_util::InputManager& inputmng, acs::vector::f2d mousePos, acs::vector::f2d& pos) {
			return false;
		}
	};

	const int stickAmounts[] = {1,2,3,4,5};
}

int game_main::StickErase::main(const game_util::COMMON_RESOURCE& resoure) {
	using namespace acex::draw;
	using namespace acex::draw::ex;

	auto draw = resoure.idraw;
	auto camera2D = resoure.camera2D;
	auto screent = resoure.iscreent;
	auto& timeKeeper = *resoure.keeper;
	auto& inputmng = *resoure.inputMng;
	auto ScreenW = resoure.screenW, ScreenH = resoure.screenH;
	bool isFirstScene;
	
	//--描画リソース定義　始--
	acs::SIACS<IRenderResource>rresourceStick;
	if(CreateIRenderResourceP(draw, resoure.ioImg, L"resource\\stick.png", &rresourceStick) == false)throw(std::exception("Resource create err"));

	acs::SIACS<IRenderResource>rresourceGameEnd;
	if (CreateIRenderResourceP(draw, resoure.ioImg, L"resource\\game_end.png", &rresourceGameEnd) == false)throw(std::exception("Resource create err"));
	sprite::SimpleSprite spriteGameEnd(draw, 1, camera2D, rresourceGameEnd);
	spriteGameEnd[0].size = {512,128};
	spriteGameEnd[0].texstate = { 0,0,1,1 };
	spriteGameEnd[0].pos = { 0,0,0 };
	primitive::Line line(draw, {1,1,0,1});
	line.setCamPro(camera2D);
	line.setLineThickness(5);

	GameManager manager(ARRAYSIZE(stickAmounts), stickAmounts, draw, camera2D, rresourceStick);
	IGamePlayer* pContraller;
	User user_controller;
	pContraller = &user_controller;
	//--描画リソース定義　終--
	game_util::DrawSyncGuarder dsg(draw);//描画リソースの定義の後に置く
BEGIN:
	//--ゲーム開始時の初期化　始--
	manager.init();
	size_t queryTimer = 0;
	spriteGameEnd[0].flag = false;
	timeKeeper.reset();
	isFirstScene = true;
	//--ゲーム開始時の初期化　終--

	while (timeKeeper.keepTime()) {

		acs::vector::i2d screenSize;
		AppBase::GetScreenSize(screenSize);
		//スクリーンサイズ変更時　処理
		if (AppBase::ScreenCheckResized()) {
			ThrowIfFalse(draw->ResizeScreenTarget({ (acs::uint)screenSize.x ,(acs::uint)screenSize.y }));
		}
		
		//--ゲームのメイン処理　始--
		//入力処理
		{
			inputmng.updateInputState();
			acs::vector::f2d mousePos = inputmng.getMousePos({ ScreenW,ScreenH }, screenSize);

			if (manager.gameHasFinished()) {
				if (inputmng.getMouseHoldingtick(0) == 1) {
					goto BEGIN;
				}
			}
			else {
				acs::vector::f2d queryPos;
				if (pContraller->getNewQuery(inputmng, mousePos, queryPos)) {
					++queryTimer;
				}
				else queryTimer = 0;

				if (queryTimer == 1) {
					line.setSegment(queryPos, queryPos);
					line.showModel();
				}
				else if (queryTimer > 1) {
					line.setSegment(line.getSegmentBegin(), queryPos);
					if (manager.sticksAreOnLine(line.getSegmentBegin(), line.getSegmentEnd()))line.setLineColor({ 1,1,0,1 });
					else line.setLineColor({ 1,0,0,1 });
				}
				else {
					if (line.isShownModel()) {
						manager.eraseStick(line.getSegmentBegin(), line.getSegmentEnd());
						if (manager.gameHasFinished())spriteGameEnd[0].flag = true;
						line.hideModel();;
					}
				}
			}
		
			if (inputmng.getKeyHoldingtick(VK_ESCAPE) == 1) {
				break;
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
			manager.update(updater);
			line.Update(updater);
			spriteGameEnd.Update(updater);
		}

		//描画
		{
			Drawer drawer(draw);
			float backGnd[] = { 0,0,0,0 };
			drawer->SetTargets(1, &screent, nullptr);
			drawer->ClearTarget(0, backGnd);
			manager.draw(drawer);
			line.Draw(drawer);
			spriteGameEnd.Draw(drawer);
		}
		//--描画関連処理　終--
	}
	return 0;
}