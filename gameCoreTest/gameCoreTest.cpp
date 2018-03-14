// gameCoreTest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <algorithm>

//
//棒が　arraySize 列 それぞれ　stickAmount　ずつ並んでいる
//加えられる変更は
//line　列目の from　番から　to　番目までが連続して並ぶとき消去できる
//棒がなくなったらゲーム終了
//条件は
// 1 <= line
// 1 <= from <= to <= stickAmount(line)
//
class GameManager {
	std::vector<std::vector<bool> > smStickAmount;
	int stickSum;
public:
	GameManager(size_t arraySize, const int stickAmounts[]) {
		smStickAmount.reserve(arraySize);
		stickSum = 0;
		for (size_t i = 0; i < arraySize; i++)
		{
			smStickAmount.push_back(std::vector<bool>(stickAmounts[i]));
			stickSum += stickAmounts[i];
			for (size_t j = 0; j < stickAmounts[i]; j++)
			{
				smStickAmount[i][j] = true;
			}
		}
	}

	bool sticksAreInLine(int line, int from, int to) {
		auto& l = smStickAmount[line - 1];
		if (from > to)return false;
		for (size_t i = from - 1; i < std::min<size_t>(l.size(), to); i++)
		{
			if (l[i] == false)return  false;
		}
	}

	bool eraseStick(int line, int from,int to) {
		if (sticksAreInLine() == false)return false;//棒が並んでないとき除外
		auto& l = smStickAmount[line -1 ];
		for (size_t i = from - 1; i < std::min<size_t>(l.size(), to); i++)
		{
			l[i] = false;
		}
		stickSum -= (to - from + 1);
		return true;
	}

	bool gameHasFinished() {
		return stickSum == 0;
	}

	void streamOut() {
		using std::cout;
		for (size_t i = 0; i < smStickAmount.size(); i++)
		{
			cout << i << ":";
			auto& l = smStickAmount[i];
			for (size_t j = 0; j < l.size(); j++)
			{
				if(l[j])cout <<  "1";
				else cout << "o";
			}
			cout << std::endl;
		}
	}
};
int stickAmounts[] = {5,6,7,4,1};
int main()
{
	GameManager game(5, stickAmounts);
	game.eraseStick(1,3,4);
	game.eraseStick(3,2,2);
	game.eraseStick(3,1,6);
	game.streamOut();
    return 0;
}

