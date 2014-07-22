#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

// 使用std命名空间
using namespace std;

#define ROW 4 // 一行有多少格（相对于grids，不是TABLE）
#define COL 4 // 一列有多少格（相对于grids，不是TABLE）
#define TABLE_ROW_LEN (ROW * 2 + 1) // TABLE一行要打印多少个字符串（TABLE是用于打印的字符串集合）
#define TABLE_COL_LEN (COL * 2 + 1) // TABLE一列要打印多少个字符串
#define TABLE_LEN (TABLE_ROW_LEN * TABLE_COL_LEN) // TABLE总共要打印多少个字符串
#define getIndexInTable(row, col) ((row * 2 + 1) * (COL * 2 + 1) + (col * 2 + 1)) // 通过行索引，列索引得到相对于TABLE中的string的索引
#define getIndexInGrids(row, col) ((row * COL) + col) // 通过行索引，列索引得到相对于grids中的Grid的索引

// 用于随机产生2或4，他们出现的几率是 2/1
const int SMALL_NUMS[] = { 2, 4, 2 };
#define SMALL_NUMS_LEN (sizeof(SMALL_NUMS)/sizeof(int))


// Grid
struct Grid {
	int row, col, num;
	int getTableIndex() {
		return getIndexInTable(row, col);
	}
};

// grids 用于存储每个格子的数据状态，不是真正的显示
Grid grids[ROW * COL];

// TABLE是用于打印的字符串集合（用于显示，相对于界面）
string TABLE[] = {
		"┌", "────", "┬", "────", "┬", "────", "┬", "────", "┐",
		"│", "    ", "│", "    ", "│", "    ", "│", "    ", "│",
		"├", "────", "┼", "────", "┼", "────", "┼", "────", "┤",
		"│", "    ", "│", "    ", "│", "    ", "│", "    ", "│",
		"├", "────", "┼", "────", "┼", "────", "┼", "────", "┤",
		"│", "    ", "│", "    ", "│", "    ", "│", "    ", "│",
		"├", "────", "┼", "────", "┼", "────", "┼", "────", "┤",
		"│", "    ", "│", "    ", "│", "    ", "│", "    ", "│",
		"└", "────", "┴", "────", "┴", "────", "┴", "────", "┘"
};

// 格子索引如下:
//		0		1		2		3
//		4		5		 6		7
//		8		9		10	11
//		12	13	14	15
void initGrids() {
	for (int r = 0; r < ROW; r++) {
		for (int c = 0; c < COL; c++) {
			int idx = getIndexInGrids(r, c);
			grids[idx].row = r;
			grids[idx].col = c;
			grids[idx].num = 0;
		}
	}
}

void printOptMsg() {
	printf("INPUT: a[←], w[↑], d[→], s[↓], e[exit]\n\n");
}

void printGameOver() {
	printf("┌───────────────────┐\n");
	printf("│     Game Over     │\n");
	printf("└───────────────────┘\n");
}

void printTable() {
	for (int i = 0; i < TABLE_LEN; i++) {
		printf("%s", TABLE[i].c_str());
		if (i%TABLE_ROW_LEN == (TABLE_ROW_LEN - 1)) {
			printf(" \n");
		}
	}
}

void generateRandomGrid() {

	int r = rand() % ROW;
	int c = rand() % COL;

	int idx = getIndexInGrids(r, c);
	if (grids[idx].num > 0) {
		generateRandomGrid();
		return;
	}

	int rand_idx = rand() % SMALL_NUMS_LEN;
	grids[idx].num = SMALL_NUMS[rand_idx];
}

void processGrids() {
	// resume table
	for (int r = 0; r < ROW; r++) {
		for (int c = 0; c < COL; c++) {
			int idx = getIndexInTable(r, c);
			TABLE[idx] = "    ";
		}
	}

	// set table
	for (int i = 0; i < (ROW * COL); i++) {
		if (grids[i].num > 0) {
			char numstr[4];
			if (grids[i].num < 10) {
				snprintf(numstr, 5, "  %d ", grids[i].num);
			} else if (grids[i].num < 100) {
				snprintf(numstr, 5, " %d ", grids[i].num);
			} else if (grids[i].num < 1000) {
				snprintf(numstr, 5, " %d", grids[i].num);
			} else {
				snprintf(numstr, 5, "%d", grids[i].num);
			}
			TABLE[grids[i].getTableIndex()] = numstr;
		}
	}
}

void handleLeftAction() {

	// 先合并（注意for循环中，外层是行r，内层是列c）
	for (int r = 0; r < ROW; r++) {
		int idx_prev = -1;
		for (int c = 0; c < (COL); c++) {
			int idx = getIndexInGrids(r, c);
			if (grids[idx].num > 0) {
				if (idx_prev >= 0) {
					if (grids[idx].num == grids[idx_prev].num) {
						// 合并数字
						grids[idx_prev].num = (2 * grids[idx].num);
						// 清理当前格子数字
						grids[idx].num = 0;

						// 重新记录
						idx_prev = -1;
						continue;
					}
				}
				// 为下一个相同数字合并做记忆
				idx_prev = idx;
			}
		}
	}

	// 从新排序
	for (int r = 0; r < ROW; r++) {
		int n = 0;
		for (int c = 0; c < COL; c++) {
			int idx = getIndexInGrids(r, c);
			int idx_new = getIndexInGrids(r, n); // 注意这里，先 r后n
			if (grids[idx].num > 0) {
				if (n != c) { // 注意这里 n 和 c 比较
					// 把数字移动到新位置
					grids[idx_new].num = grids[idx].num;
					// 清理当前格子数字
					grids[idx].num = 0;
				}
				n++;
			}
		}
	}

}

void handleUpAction() {

	// 先合并（注意for循环中，外层是列c，内层是行r）
	for (int c = 0; c < COL; c++) {
		int idx_prev = -1;
		for (int r = 0; r < ROW; r++) {
			int idx = getIndexInGrids(r, c);
			if (grids[idx].num > 0) {
				if (idx_prev >= 0) {
					if (grids[idx].num == grids[idx_prev].num) {
						// 合并数字
						grids[idx_prev].num = (2 * grids[idx].num);
						// 清理当前格子数字
						grids[idx].num = 0;

						// 重新记录
						idx_prev = -1;
						continue;
					}
				}
				// 为下一个相同数字合并做记忆
				idx_prev = idx;
			}
		}
	}

	// 从新排序
	for (int c = 0; c < COL; c++) {
		int n = 0;
		for (int r = 0; r < ROW; r++) {
			int idx = getIndexInGrids(r, c);
			int idx_new = getIndexInGrids(n, c); // 注意这里，先 n后c
			if (grids[idx].num > 0) {
				if (n != r) { // 注意这里 n 和 r 比较
					// 把数字移动到新位置
					grids[idx_new].num = grids[idx].num;
					// 清理当前格子数字
					grids[idx].num = 0;
				}
				n++;
			}
		}
	}

}

void handleRightAction() {

	// 先合并（注意for循环中，外层是行r，内层是列c）
	for (int r = 0; r < ROW; r++) {
		int idx_prev = -1;
		for (int c = (COL - 1); c >= 0; c--) {
			int idx = getIndexInGrids(r, c);
			if (grids[idx].num > 0) {
				if (idx_prev >= 0) {
					if (grids[idx].num == grids[idx_prev].num) {
						// 合并数字
						grids[idx_prev].num = (2 * grids[idx].num);
						// 清理当前格子数字
						grids[idx].num = 0;

						// 重新记录
						idx_prev = -1;
						continue;
					}
				}
				// 为下一个相同数字合并做记忆
				idx_prev = idx;
			}
		}
	}

	// 从新排序
	for (int r = 0; r < ROW; r++) {
		int n = (COL - 1);
		for (int c = (COL - 1); c >= 0; c--) {
			int idx = getIndexInGrids(r, c);
			int idx_new = getIndexInGrids(r, n); // 注意这里，先 r后n
			if (grids[idx].num > 0) {
				if (n != c) { // 注意这里 n 和 c 比较
					// 把数字移动到新位置
					grids[idx_new].num = grids[idx].num;
					// 清理当前格子数字
					grids[idx].num = 0;
				}
				n--;
			}
		}
	}

}

void handleDownAction() {

	// 先合并（注意for循环中，外层是列c，内层是行r）
	for (int c = 0; c < COL; c++) {
		int idx_prev = -1;
		for (int r = (ROW - 1); r >= 0; r--) {
			int idx = getIndexInGrids(r, c);
			if (grids[idx].num > 0) {
				if (idx_prev >= 0) {
					if (grids[idx].num == grids[idx_prev].num) {
						// 合并数字
						grids[idx_prev].num = (2 * grids[idx].num);
						// 清理当前格子数字
						grids[idx].num = 0;

						// 重新记录
						idx_prev = -1;
						continue;
					}
				}
				// 为下一个相同数字合并做记忆
				idx_prev = idx;
			}
		}
	}

	// 从新排序
	for (int c = 0; c < COL; c++) {
		int n = (ROW - 1);
		for (int r = (ROW - 1); r >= 0; r--) {
			int idx = getIndexInGrids(r, c);
			int idx_new = getIndexInGrids(n, c); // 注意这里，先 n后c
			if (grids[idx].num > 0) {
				if (n != r) { // 注意这里 n 和 r 比较
					// 把数字移动到新位置
					grids[idx_new].num = grids[idx].num;
					// 清理当前格子数字
					grids[idx].num = 0;
				}
				n--;
			}
		}
	}

}

int getGridCount() {
	int grid_count = 0;
	for (int i = 0; i < (ROW * COL); i++) {
		if (grids[i].num > 0) {
			grid_count++;
		}
	}
	return grid_count;
}

// a[←], w[↑], d[→], s[↓], e[exit]
int handleAction(char *act) {
	if (!strcmp(act, "a")) {
		handleLeftAction();
	} else if (!strcmp(act, "w")) {
		handleUpAction();
	} else if (!strcmp(act, "d")) {
		handleRightAction();
	} else if (!strcmp(act, "s")) {
		handleDownAction();
	} else if (!strcmp(act, "e") || !strcmp(act, "exit")) {
		return -1;
	}

	return 0;
}

void displayNewTable() {
	generateRandomGrid();
	processGrids();
	printTable();
}

int main(void) {
	char buf[64];

	printf("\nWelcome to 2048, let`s begin now. [Y/N] ");
	scanf("%s", buf);

	if (!strcmp(buf, "Y")
			|| !strcmp(buf, "y")
			|| !strcmp(buf, "YES")
			|| !strcmp(buf, "yes")) {

		initGrids();
		printf("\n\n");

		// grid 0，默认在第一行，第四列
		int idx0 = getIndexInGrids(0, 3);
		grids[idx0].num = 2;

		// 显示新（下一个）界面
		displayNewTable();

		// 打印提示信息
		printOptMsg();

		while ((scanf("%s", buf))) {
			if (handleAction(buf) < 0) {
				break;
			}
			displayNewTable();

			// FIXME, 如果已经满个则宣布游戏结束（这里的算法不严谨）
			if ((getGridCount() + 1) >= (ROW * COL)) {
				printGameOver();
				break;
			}

			printOptMsg();
		}
	} else {
		printf("[%s] Exit the game...\n\n", buf);
	}

}
