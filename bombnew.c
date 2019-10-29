#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROW 16
#define COLUMN 30
#define MINE_AMOUNT 99
#define DISCOVERED_MINE_SHAPE 'M' 
#define UNKNOWN_MINE_SHAPE '*'
#define CLEAR_SHAPE ' '// ����ڰ� �߰��� �� ĭ
#define UNKNOWN '0' //����ڰ� �߰����� ���� �� ĭ

void print(char map[ROW][COLUMN]);
void show(char map[ROW][COLUMN]);
void create_map(char map[ROW][COLUMN]);
void mining(char map[ROW][COLUMN], int a, int b);
void numbering(char map[ROW][COLUMN]);
int minecheck(char map[ROW][COLUMN], int x, int y);
int select(char map[ROW][COLUMN], int x, int y, int life);
int chain(char map[ROW][COLUMN], int x, int y);
void discover(char map[ROW][COLUMN], int x, int y);
int kill_mine(char map[ROW][COLUMN], int x, int y, int life);
void boom(char map[ROW][COLUMN], int x, int y);
int scanner(char map[ROW][COLUMN], int life);
int Showmenu();

static int mode = 0;//0 : select, 1 : kill_mine

void print(char map[ROW][COLUMN]) {// ���� ������ ǥ�����ִ� �Լ�(�����ڿ�����, ������ ��� ���°� ��Ÿ����)
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COLUMN; j++) {
			printf("%c", map[i][j]);
		}
		printf("\n");
	}
	return;
}

void show(char map[ROW][COLUMN]) {// ���� ������ ����, �÷��̾��� ��Ȳ�� �°� ������ִ� �Լ�, ������ ���� �о� '��'�� ���ڷ� �ٲ� ��� �� ������ ����� ����Ʈ�Ѵ�.
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COLUMN; j++) {
			char temp = map[i][j];
			if (temp == UNKNOWN_MINE_SHAPE || temp == UNKNOWN || ('a' < temp && temp < 'j')) {
				printf("��"); //Ž�� ���� �κ�
			}
			else if (temp == CLEAR_SHAPE) {
				printf("��"); //Ž�縦 ������ ������ �κ�
			}
			else if (temp == 'Q') {
				printf("��"); //���� (kill��忡�� ���ڰ� �ƴ� �κ��� kill�� ���)
			}
			else if ('0' < temp && temp < '9') {
				printf(" %c", temp); //��ó ���ڰ���
			}
			else if (temp == DISCOVERED_MINE_SHAPE) {
				printf("��"); //���� ����(�߰�) �Ϸ�
			}
		}
		printf("\n");
	}
}

void create_map(char map[ROW][COLUMN]) {// ������ �ʱ�ȭ�� ����ϴ� �Լ�
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COLUMN; j++) {
			map[i][j] = UNKNOWN;
		}
	}
	return;
}


void mining(char map[ROW][COLUMN], int a, int b) {// ������ ���ڸ� �ż��ϴ� ����, ù �Է����� a, b�� �޾�, �� ĭ�� �����ϰ� ���� ����
	srand((unsigned)time(NULL));
	int x = 0, y = 0;
	for (int i = 0; i < MINE_AMOUNT; i++) {
		x = rand() % ROW;
		y = rand() % COLUMN;
		if (map[x][y] == UNKNOWN && !((x == a) && (y == b))) {
			map[x][y] = UNKNOWN_MINE_SHAPE;
		}
		else {
			i--;
			continue;
		}
	}
	return;
}

void numbering(char map[ROW][COLUMN]) {// ������ �ż��� ���ڸ� ���� ���ڸ� ä�� �ִ� ����
	int temp = 0;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COLUMN; j++) {
			if (map[i][j] == UNKNOWN_MINE_SHAPE) continue;
			temp = minecheck(map, i, j);
			if (temp == 0) continue;
			map[i][j] = (char)temp + 'a';//������ ���ڴ� ����, �������� ���� ���ڴ� b, c, d...�� ���� ���ĺ����� ǥ��ȴ�.
		}
	}
}

int minecheck(char map[ROW][COLUMN], int x, int y) {//������ ĭ ���� 8ĭ�� Ž���Ͽ� ������ ���� �����ϴ� �Լ�, minecheck�� ����
	int hor[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int ver[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int result = 0;
	int xi = 0, yi = 0;
	for (int i = 0; i < 8; i++) {
		xi = x + hor[i], yi = y + ver[i];
		if (xi < 0 || xi >= ROW || yi < 0 || yi >= COLUMN) continue;
		if (map[xi][yi] == UNKNOWN_MINE_SHAPE) result++;
	}
	return result;
}

int select(char map[ROW][COLUMN], int x, int y, int life) {//���� �������� ���� ���ڶ�� 0�� ����, �������� 1�� �����Ѵ�
	char temp = map[x][y];
	if (temp == UNKNOWN_MINE_SHAPE) {
		boom(map, x, y);
		life--;
	}
	if (temp == UNKNOWN) {
		chain(map, x, y);
	}
	else if ('a' < temp && temp < 'j') {
		map[x][y] = map[x][y] - 'a' + '0';
	}
	return life;
}

int chain(char map[ROW][COLUMN], int x, int y) {//ã�� ��忡�� ��ư�� ������ �� �˷����� ���� �� ĭ�̾��� ���, ���������� �� ĭ�� ���ڵ��� Ž��
	int hor[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int ver[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int xi = 0, yi = 0;
	char temp = map[x][y];
	if (temp == CLEAR_SHAPE || temp == DISCOVERED_MINE_SHAPE || temp == UNKNOWN_MINE_SHAPE || ('0' < temp && temp < '9')) return 1; // �̹� ����� ĭ�̳� ���ڸ� return
	discover(map, x, y);// �̹� ĭ�� ����
	for (int i = 0; i < 8; i++) {//�ֺ� 8ĭ�� ���������� ����
		xi = x + hor[i], yi = y + ver[i];
		if (xi < 0 || xi >= ROW || yi < 0 || yi >= COLUMN) continue;
		if (temp == UNKNOWN)
			chain(map, xi, yi);
	}
	return 1;
}

void discover(char map[ROW][COLUMN], int x, int y) {// �˷����� ���� ��ĭ�̰ų�, �˷����� ���� ������ ��� �׸� ����ڿ��� �巯���� �Լ�, chain�� ����
	char temp = map[x][y];
	if (temp == UNKNOWN) {
		map[x][y] = CLEAR_SHAPE;
		return;
	}
	if ('a' < temp && temp < 'j') {
		map[x][y] = map[x][y] - 'a' + '0';
		return;
	}
}

int kill_mine(char map[ROW][COLUMN], int x, int y, int life) {
	char temp = map[x][y];
	if (temp == UNKNOWN_MINE_SHAPE)
		map[x][y] = DISCOVERED_MINE_SHAPE;
	else {
		boom(map, x, y);
		life--;
	}
	return life;
}

void boom(char map[ROW][COLUMN], int x, int y) {
	map[x][y] = 'Q';
	return;
}

int scanner(char map[ROW][COLUMN], int life) { //��� ���� �� ��,�� ����.
	int inputx = 0, inputy = 0;
	int result;
	char input[5];
	if (mode) { printf("���� ��� : kill_mine (��� ������ 0�� �Է��ϼ���) \n"); }
	else { printf("���� ��� : select (��庯���� 0�� �Է��ϼ���) \n"); }
	printf("������ �� (�ִ� : %d) : ", ROW);
	scanf("%d", &inputx);
	if (inputx == 0) {
		if (mode) mode = 0;
		else mode = 1;
		return life;
	}
	printf("������ �� (�ִ� : %d) : ", COLUMN);
	scanf("%d", &inputy);
	if (inputy == 0) {
		if (mode) mode = 0;
		else mode = 1;
		return life;
	}
	inputx -= 1;
	inputy -= 1;
	if (mode) {
		result = kill_mine(map, inputx, inputy, life);
	}
	else {
		result = select(map, inputx, inputy, life);
	}
	return result;
}

int Showmenu() { //���̵� ���� �޴�
	int difficulty = 0;

	while (1) {
		printf("���̵��� �����ϼ���(easy(1), normal(2), crazy(3), ����(0)): ");
		scanf("%d", &difficulty);

		switch (difficulty) {
		case 1:
			return 10;
			break;
		case 2:
			return 5;
			break;
		case 3:
			return 1;
			break;
		case 0:
			printf("select mode: Ž��(���� ���� �� LIFE ����)\n");
			printf("kill mode: ��������(���� �� ���� �� LIFE ����)\n");
			break;
		default:
			printf("���̵��� �ùٸ��� �Է��ϼ���\n");
			break;
		}
	}
}

int main(void) {
	//�� ���� ã�⿡�� �� ���� ��尡 �ֽ��ϴ�
	//���� kill_mine ����Դϴ�
	//�� ���� ���� ����ã�⿡ ������ ���콺 ��ư�� ��� ���� �����Ǹ�, ���ڶ�� �����Ǵ� ��ǥ�� ������ �� �ֽ��ϴ�
	//���� ���ڶ��, �������� �巯����, ���ڰ� �ƴ϶��, ���ӿ��� �й��մϴ�.
	//�� ������ select ����Դϴ�
	//�� ���� ���� ����ã���� ���� ���콺 ��ư�� ��� ���� �����Ǹ�, ��ǥ�� �ϳ� ������ �� �ֽ��ϴ�.
	//���� �� ��ǥ�� ���ڶ�� ���ӿ��� �й��մϴ�
	//���ڰ� �ƴ϶��, �� ĭ�� ���ڸ� �巯���ų�, ���ڰ� ���� �� ĭ�̶��, ���������� ĭ���� Ž���Ͽ� ���ڵ��� �巯���ϴ�.
	//mode => 0 : select, 1 : kill_mine

	char map[ROW][COLUMN];
	int startx = 0, starty = 0;
	int life = Showmenu();
	char initialinput[5];

	printf("����ã�⸦ �����մϴ�.\n");
	printf("���� LIFE: %d\n", life);
	printf("������ ���� �Է��ϼ��� (�ִ� %d) : ", ROW);
	scanf("%d", &startx);
	printf("������ ���� �Է��ϼ��� (�ִ� %d) : ", COLUMN);
	scanf("%d", &starty);
	create_map(map);
	mining(map, startx - 1, starty - 1);
	numbering(map);
	select(map, startx - 1, starty - 1, life);
	show(map);
	while (life) {
		printf("���� LIFE: %d\n", life);
		life = scanner(map, life);
		show(map);
	}

	return 0;
}