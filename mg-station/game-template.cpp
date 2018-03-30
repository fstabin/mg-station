#include "acex_draw_ex\include\acex_draw_ex.h"

#include "game-template.h"//�Q�[���N���X�錾�w�b�_

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

	//--���\�[�X��`�@�n--
	//�����ɃR�[�h��ǉ�
	//--���\�[�X��`�@�I--
	game_util::DrawSyncGuarder dsg(draw);//���\�[�X�̒�`�̌�ɒu��

BEGIN:
	{
		//--�Q�[���J�n���̏������@�n--
		timeKeeper.reset();
		isFirstScene = true;
		//--�Q�[���J�n���̏������@�I--

		while (timeKeeper.keepTime()) {
			acs::vector::i2d screenSize;
			AppBase::GetScreenSize(screenSize);
			//�X�N���[���T�C�Y�ύX���@����
			if (AppBase::ScreenCheckResized()) {
				if (false == draw->ResizeScreenTarget({ (acs::uint)screenSize.x ,(acs::uint)screenSize.y }))throw(std::exception("screen resize err"));
			}

			//--�Q�[���̃��C�������@�n--
			{
				inputmng.updateInputState();

				/*
				if (inputmng.getKeyHoldingtick(KEY_CODE) == 1) {
					�{�^���������ꂽ�u�Ԃ̏���
				}
				if(inputmng.getKeyHoldingtick(KEY_CODE) > 0){
					�{�^����������Ă���Ԃ����Ƃ̏���
				}
				if (inputmng.getKeyHoldingtick(KEY_CODE) == -1) {
				�{�^���������ꂽ�u�Ԃ̏���
				}
				*/

				if (inputmng.getKeyHoldingtick(VK_ESCAPE) == 1) {
					//esc�Ŗ߂�
					return std::pair<int, game_util::GameCommand>(0, game_util::GameCommand::EndApp);
				}
			}
			//--�Q�[���̃��C�������@�I--


			//--�`��֘A�����@�n--
			//�\��
			if (isFirstScene) isFirstScene = false;
			else draw->Present(0);

			//�`��f�[�^�X�V
			{
				Updater updater(draw);
				//�f�[�^�X�V
			}

			//�`��
			{
				Drawer context(draw);
				//float backGnd[] = { 0,0,0,0 };
				//context->SetTargets(1, &screent, nullptr);
				//context->ClearTarget(0, backGnd);
				//�I�u�W�F�N�g�`��
			}
			//--�`��֘A�����@�I--
		}
		return std::pair<int, game_util::GameCommand>(0, game_util::GameCommand::EndApp);
	}
}
