#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <SFML/Audio.hpp>

using namespace sf;

struct Player {
	RectangleShape sprite;
	int speed;
	int score;
	int life;
	float x, y;
};
struct Enemy {
	RectangleShape sprite;
	int speed;
	int score;
	int life;
	int respawn_time;
	SoundBuffer explosion_buffer;
	Sound explosion_sound;
};
// 총알
struct Bullet {
	RectangleShape sprite;
	int speed;
	int is_fired; // 발사 여부
};
struct Textures {
	Texture bg;       // 배경 이미지
	Texture enemy;
	Texture gameover; // 게임오버 이미지
	Texture player;   // 플레이어 이미지
	Texture bullet;
};

// obj1과 obj2의 충돌 여부 / 충돌 시 return 1 / 충돌x 시 return 0.
int is_collide(RectangleShape obj1, RectangleShape obj2)
{
	return obj1.getGlobalBounds().intersects(obj2.getGlobalBounds());

}

// 전역변수
const int ENEMY_NUM = 10;                  // enemy 최대 개수
const int W_WIDTH = 800, W_HEIGHT = 480;   // 창의 크기
const int GO_WIDTH = 203, GO_HEIGHT = 106; // 게임오버 이미지 크기

int main(void) {
	struct Textures t;
	t.bg.loadFromFile("./resources/background.jpg"); 
	t.enemy.loadFromFile("./resources/enemy.png");
	t.gameover.loadFromFile("./resources/gameover.png");
	t.player.loadFromFile("./resources/player.png");
	t.bullet.loadFromFile("./resources/ring.png");
	
	// 윈도우 창 생성
	RenderWindow window(VideoMode(W_WIDTH, W_HEIGHT), "Let's win with the Jesus!");
	window.setFramerateLimit(60);

	srand(time(0));

	long start_time = clock();  //게임 시작 시간
	long spent_time;			//게임 진행 시간
	int is_gameOver = 0;        //게임 오버

	//BGM
	SoundBuffer BGM_buffer;
	BGM_buffer.loadFromFile("./resources/TownTheme.ogg");
	Sound BGM_sound;
	BGM_sound.setBuffer(BGM_buffer);
	BGM_sound.setLoop(1);   // BGM 무한반복
	BGM_sound.play();

	//폰트 
	Font font;
	font.loadFromFile("C:\\Windows\\Fonts\\Arial.ttf");

	//텍스트
	Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(Color(255, 255, 255));
	text.setPosition(0, 0);
	char info[40];
	text.setString("score");

	//배경
	Sprite bg_sprite;
	bg_sprite.setTexture(t.bg);
	bg_sprite.setPosition(0, 0);

	//게임오버
	Sprite gameover_sprite;
	gameover_sprite.setTexture(t.gameover);
	gameover_sprite.setPosition((W_WIDTH-GO_WIDTH)/2, (W_HEIGHT-GO_HEIGHT)/2); // 정가운데 위치 공식

	//player
	struct Player player;
	player.sprite.setSize(Vector2f(80, 90));
	player.sprite.setTexture(&t.player);
	player.sprite.setPosition(100, 100);
	player.x = player.sprite.getPosition().x;
	player.y = player.sprite.getPosition().y;
	player.speed = 7;
	player.score = 0;
	player.life = 5;

	// 총알
	struct Bullet bullet;
	bullet.sprite.setSize(Vector2f(40, 40));
	bullet.sprite.setPosition(player.x + 50, player.y + 15);
	bullet.speed = 20;
	bullet.is_fired = 0;
	bullet.sprite.setTexture(&t.bullet);

	//enemy
	struct Enemy enemy[ENEMY_NUM];

	//enemy 초기화
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		//TODO : 굉장히 비효율적인 코드이므로 나중에 refactoring
		enemy[i].explosion_buffer.loadFromFile("./resources/bubbles-single2.wav");
		enemy[i].explosion_sound.setBuffer(enemy[i].explosion_buffer);
		enemy[i].score = 100;
		enemy[i].respawn_time = 10;

		enemy[i].sprite.setTexture(&t.enemy);
		enemy[i].sprite.setSize(Vector2f(150, 150));
		enemy[i].sprite.setPosition(rand()% 300 + W_WIDTH*0.9, rand() % 380);
		enemy[i].life = 1;
		enemy[i].speed = -(rand() % 4 + 1);
	}

	// 윈도우가 열려 있을 때 까지 반복
	while (window.isOpen()) 
	{ 
		spent_time = clock() - start_time;
		player.x = player.sprite.getPosition().x;
		player.y = player.sprite.getPosition().y;
		Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case Event::Closed:// 종료(x) 버튼을 누르면
				window.close();// 윈도우를 닫는다
				break;
				//키보드를 눌렀을 때 (누른 순간만 감지)
			case Event::KeyPressed:
			{
				////space 누르면 모든  enemy 재출현
				//if (event.key.code == Keyboard::Space)
				//{
				//	for (int i = 0; i < ENEMY_NUM; i++)
				//	{
				//		enemy[i].sprite.setSize(Vector2f(70, 70));
				//		enemy[i].sprite.setFillColor(Color::Yellow);
				//		enemy[i].life = 1;
				//		enemy[i].sprite.setPosition(rand() % 300 + W_WIDTH * 0.9, rand() % 380);
				//		enemy[i].speed = -(rand() % 4 + 1);

				//	}
				//}
				break;
			}

			}

		}

		//PLAYER UPDATE
		//player 방향키 start
		if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
		{
			player.sprite.move(-player.speed, 0);
		}
		if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
		{
			player.sprite.move(player.speed, 0);
		}
		if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up))
		{
			player.sprite.move(0, -player.speed);
		}
		if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down))
		{
			player.sprite.move(0, player.speed);
		} 

		//player 이동범위 제한
		if (player.x < 0)
		{
			player.sprite.setPosition(0, player.y);
		}
		else if (player.x > W_WIDTH - 80)
		{
			player.sprite.setPosition(W_WIDTH - 80, player.y);
		}
		if (player.y < 0)
		{
			player.sprite.setPosition(player.x, 0);
		}
		else if (player.y > W_HEIGHT - 90)
		{
			player.sprite.setPosition(player.x, W_HEIGHT - 90);
		}

		//BULLET UPDATE
		if (Keyboard::isKeyPressed(Keyboard::Space))
		{
			//총알이 발사되어있지 않다면
			if (!bullet.is_fired)
			{
				bullet.sprite.setPosition(player.x + 50, player.y + 15);
				bullet.is_fired = 1;
			}
		}
		// TODO : 총알이 한 번만 발사되는 버그 Modify
		if (bullet.is_fired)
		{
			bullet.sprite.move(bullet.speed, 0);
			if (bullet.sprite.getPosition().x > W_WIDTH)
			{
				bullet.is_fired = 0;
			}
		}

		//ENEMY UPDATE
		for (int i = 0; i < ENEMY_NUM; i++)
		{
			// 8초마다 enemy가 리스폰
			if (spent_time % (1000* enemy[i].respawn_time) < 1000/60 + 1)
			{
				enemy[i].sprite.setSize(Vector2f(150, 150));
				enemy[i].life = 1;
				enemy[i].sprite.setPosition(rand() % 300 + W_WIDTH * 0.9, rand() % 380);
				enemy[i].sprite.setTexture(&t.enemy);
				// 10초마다 enemy_spped += 1
				enemy[i].speed = -(rand() % 4 + 1);
				enemy[i].speed = -(rand() % 1 + 1 + (spent_time / 1000 / enemy[i].respawn_time));
			}

			if (enemy[i].life > 0)
			{
				//player - enemy와의 충돌
				if (is_collide(player.sprite, enemy[i].sprite))
				{
					enemy[i].life -= 1;
					player.score += enemy[i].score;

					// TODO : 코드 refactoring 필요
					if (enemy[i].life == 0)
					{
						enemy[i].explosion_sound.play();
					}
				}
				//enemy가 왼쪽에 진입하려는 순간
				else if (enemy[i].sprite.getPosition().x < 0)
				{
					player.life -= 1;
					enemy[i].life = 0;
				}
				enemy[i].sprite.move(enemy[i].speed, 0);

				//bullet - enemy와의 충돌
				if (is_collide(bullet.sprite, enemy[i].sprite))
				{
					enemy[i].life -= 1;
					player.score += enemy[i].score;

					// TODO : 코드 refactoring 필요
					if (enemy[i].life == 0)
					{
						enemy[i].explosion_sound.play();
					}
					bullet.is_fired = 0;
				}
			}
		}

		if (player.life <= 0)
		{
			is_gameOver = 1;
		}

		//점수 적용
		sprintf(info, "Life : %d | Score : %d | Time : %dsec", player.life, player.score, spent_time/1000);
		text.setString(info);

		window.clear(Color::Black);
		window.draw(bg_sprite);

		//draw는 나중에 호출할수록 우선순위가 높아짐
		for (int i = 0; i < ENEMY_NUM; i++)
		{
			if (enemy[i].life > 0)
			{
				window.draw(enemy[i].sprite);
			}
		}
		
		window.draw(player.sprite);
		window.draw(text);
		if (bullet.is_fired)
		{
			window.draw(bullet.sprite);
		}

		if (is_gameOver)
		{
			window.draw(gameover_sprite);
			// TODO : 게임이 멈추는 것을 구현할 것
		}

		window.display();
	}
	return 0;
}
