#include<windows.h>
#include"icb_gui.h"
#include"ic_image.h"

int F1, SLE;
int keypressed = -1;

int count = 0;

int dir = 0;
int life = 2;

int frogX = 296;
int frogY = 613;

bool flag = false;

ICBYTES entryScreen;
ICBYTES frog, frogStill, frogBig;
HANDLE hTimer = NULL;
HANDLE hTimerQueue = NULL;
HANDLE hMutex;
DWORD DW;

int count1, count2, count3, count5 = 0, gedikCounter = 0;

void gamePlay(void*);
void drawCar1(void);
void drawCar2(void);
void drawCar3(void);
void drawCar4(void);
void drawCar5(void);
void timeLimit(void);
void drawFrog(void);
void drawShortLog(void);
void drawLongLog(void);
void drawFrog2(void);
void drawMiddleLog(void);

void ICGUI_Create()
{
	ICG_MWSize(700, 750);
	ICG_MWTitle("FROGGER");
}

void WhenKeyPressed(int k)
{
	keypressed = k;
}

void frogStart()
{
	ICBYTES back, background;

	ReadImage("sprites.bmp", frog);
	ReadImage("frogger.bmp", back);

	//// kurabaganin ilk yeri ve hali cizimi
	Copy(frog, 1, 1, 15, 15, frogStill);
	MagnifyX3(frogStill, frogBig);
	PasteNon0(frogBig, frogX, frogY, entryScreen);
	//// kurabaganin ilk yeri ve hali cizimi

	DisplayImage(F1, entryScreen);
}

void deathAnimation()
{
	ICBYTES deaths, imgs, img;

	ReadImage("sprites.bmp", imgs);

	for (int i = 0; i < 100; i++)
	{
		//// Death 1
		Copy(imgs, 2, 80, 17, 15, img);
		MagnifyX3(img, deaths);
		PasteNon0(deaths, frogX, frogY, entryScreen);

		DisplayImage(F1, entryScreen);
	}

	for (int i = 0; i < 100; i++)
	{
		//// Death 2
		Copy(imgs, 19, 80, 15, 14, img);
		MagnifyX3(img, deaths);
		PasteNon0(deaths, frogX, frogY, entryScreen);

		DisplayImage(F1, entryScreen);
	}

	for (int i = 0; i < 100; i++)
	{
		//// Death 4
		Copy(imgs, 109, 79, 16, 17, img);
		MagnifyX3(img, deaths);
		PasteNon0(deaths, frogX, frogY - 10, entryScreen);

		DisplayImage(F1, entryScreen);
	}

	FillRect(entryScreen, frogX, frogY - 5, 50, 50, 0x000000); //death animation delete

	frogX = 296;
	frogY = 613;

	frogStart();

	keypressed = NULL;
	flag = false;

}

void startGame(void*) // oyunu baslatan thread, tusa basilinca durur
{
	DWORD dw;

	ICBYTES frogBig;
	ICBYTES title, press;

	ReadImage("frogger.bmp", entryScreen);

	ReadImage("frogger_title.bmp", title);
	ReadImage("press_text.bmp", press);

	PasteNon0(title, 40, 200, entryScreen);
	PasteNon0(press, 70, 470, entryScreen);

	DisplayImage(F1, entryScreen);

	hTimerQueue = CreateTimerQueue();
	keypressed = NULL;

	life = 2;

	while (true)
	{
		ICG_SetWindowText(SLE, "");
		ICG_printf(SLE, "%d ", keypressed);

		if (keypressed > 0)// any key
		{
			keypressed = NULL;

			hMutex = CreateMutex(NULL, FALSE, NULL);

			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gamePlay, NULL, 0, &dw);

			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)timeLimit, NULL, 0, &dw);

			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)drawCar1, NULL, 0, 7000, 0);
			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)drawCar2, NULL, 0, 5000, 0);
			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)drawCar3, NULL, 0, 5500, 0);
			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)drawCar4, NULL, 0, 13000, 0);
			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)drawCar5, NULL, 0, 7000, 0);

			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)drawFrog, NULL, 0, 7000, 0);
			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)drawShortLog, NULL, 0, 5000, 0);
			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)drawLongLog, NULL, 0, 6000, 0);
			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)drawFrog2, NULL, 0, 7000, 0);
			CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)drawMiddleLog, NULL, 0, 5000, 0);
			break;
		}
	}
}

void Restart()
{

	if (life == -1)
	{
		FillRect(entryScreen, 80, 350, 500, 30, 0x4B4242); // gri ekran

		Impress12x20(entryScreen, 80, 355, "Restart in 5", 0xFFFFFF); // yazi
		DisplayImage(F1, entryScreen); // goruntule

		Sleep(5000); // 5 sn beklet

		DeleteTimerQueue(hTimerQueue); // timerlari kapat

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startGame, NULL, 0, &DW); // bastan baslat
	}
}

void Win()
{
	if (gedikCounter == 5 && life != -1)
	{
		FillRect(entryScreen, 80, 350, 500, 30, 0x4B4242); // gri ekran

		Impress12x20(entryScreen, 80, 355, "You Win!!!!", 0xFFFFFF); // yazi
		DisplayImage(F1, entryScreen); // goruntule

		Sleep(5000); // 5 sn beklet

		DeleteTimerQueue(hTimerQueue); // timerlari kapat
	}
}

void timeLimit()
{
	DWORD dw;

	FillRect(entryScreen, 155, 675, 400, 20, 0x146450);
	DisplayImage(F1, entryScreen);

	int c = 0;
	while (c < 400)
	{
		if (life == -1 || gedikCounter == 5)
		{
			break;
		}

		FillRect(entryScreen, 155, 675, 1 + c, 20, 0x000000);
		DisplayImage(F1, entryScreen);
		Sleep(90);
		c += 1;
	}

	if (!flag)
	{
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)deathAnimation, NULL, 0, &dw);

		if (life == 2)
		{
			FillRect(entryScreen, 59, 655, 40, 40, 0x000000);
		}
		else if (life == 1)
		{
			FillRect(entryScreen, 10, 655, 44, 40, 0x000000);
		}

		life--;
		flag = true;
		Restart();
	}

	if (life > -1)
	{
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)timeLimit, NULL, 0, &dw);
	}
}

void drawMiddleLog()
{
	ICBYTES frog2, frogStill;
	ICBYTES frogBig;
	ReadImage("sprites.bmp", frog2);

	ICBYTES longLog, logs, log;
	ReadImage("middleLog.bmp", logs);

	int log_x = 0;
	int log_y = 110;

	while (true)
	{
		// odunun eski hali silinsin diye
		FillRect(entryScreen, log_x, log_y, 100, 40, 0x1830ac);

		Copy(logs, 1, 1, 45, 15, log);
		MagnifyX3(log, longLog);
		PasteNon0(longLog, log_x, log_y, entryScreen);

		if (frogX + 26 > log_x + 5 && frogX + 26 < log_x + 130 && frogY + 25 < log_y + 36 && frogY + 25 > log_y)
		{
			if (dir == 0)
			{
				Copy(frog2, 1, 1, 15, 15, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
			if (dir == 1)
			{
				Copy(frog2, 111, 2, 12, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
			if (dir == 2)
			{
				Copy(frog2, 74, 4, 13, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
			if (dir == 3)
			{
				Copy(frog2, 38, 2, 12, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
		}

		DisplayImage(F1, entryScreen);

		log_x += 2;
		if (frogX + 26 > log_x + 5 && frogX + 26 < log_x + 130 && frogY + 25 < log_y + 36 && frogY + 25 > log_y)
		{
			frogX += 2;
		}

		Sleep(15);

		if (log_x > 700) // ekrandan cikinca thread sonlansin
		{
			break;
		}
	}
}

void drawFrog2()
{
	ICBYTES bottomFlower, flowers, flower;
	ReadImage("sprites.bmp", flowers);

	ICBYTES frog, frogStill;
	ICBYTES frogBig;

	int frog_x = 650;
	int frog_y = 160;

	int frog2_x = 700;
	int frog2_y = 160;

	while (true)
	{
		for (int i = 0; i < 10; i++)
		{
			// cicegin eski hali silinsin diye
			FillRect(entryScreen, frog_x + 8, frog_y - 3, 107, 40, 0x1830ac);

			Copy(flowers, 1, 155, 15, 10, flower);
			MagnifyX3(flower, bottomFlower);
			PasteNon0(bottomFlower, frog_x, frog_y, entryScreen);
			PasteNon0(bottomFlower, frog2_x, frog2_y, entryScreen);

			if (frogX + 26 > frog_x + 5 && frogX + 26 < frog_x + 95 && frogY + 25 < frog_y + 35 && frogY + 25 > frog_y)
			{
				if (dir == 0)
				{
					Copy(flowers, 1, 1, 15, 15, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 1)
				{
					Copy(flowers, 111, 2, 12, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 2)
				{
					Copy(flowers, 74, 4, 13, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 3)
				{
					Copy(flowers, 38, 2, 12, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
			}

			DisplayImage(F1, entryScreen);

			frog_x -= 2;
			frog2_x -= 2;

			if (frogX + 26 > frog_x + 5 && frogX + 26 < frog_x + 95 && frogY + 25 < frog_y + 35 && frogY + 25 > frog_y)
			{
				frogX -= 2;
			}

			Sleep(15);

		}

		for (int i = 0; i < 10; i++)
		{
			// cicegin eski hali silinsin diye
			FillRect(entryScreen, frog_x + 8, frog_y - 3, 107, 40, 0x1830ac);

			Copy(flowers, 18, 154, 16, 12, flower);
			MagnifyX3(flower, bottomFlower);
			PasteNon0(bottomFlower, frog_x, frog_y, entryScreen);
			PasteNon0(bottomFlower, frog2_x, frog2_y, entryScreen);

			if (frogX + 26 > frog_x + 5 && frogX + 26 < frog_x + 95 && frogY + 25 < frog_y + 35 && frogY + 25 > frog_y)
			{
				if (dir == 0)
				{
					Copy(flowers, 1, 1, 15, 15, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 1)
				{
					Copy(flowers, 111, 2, 12, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 2)
				{
					Copy(flowers, 74, 4, 13, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 3)
				{
					Copy(flowers, 38, 2, 12, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
			}

			DisplayImage(F1, entryScreen);

			frog_x -= 2;
			frog2_x -= 2;
			if (frogX + 26 > frog_x + 5 && frogX + 26 < frog_x + 95 && frogY + 25 < frog_y + 35 && frogY + 25 > frog_y)
			{
				frogX -= 2;
			}

			Sleep(15);

		}

		if (frog_x < -55 && frog2_x < -55) // ekrandan cikinca thread sonlansin
		{
			break;
		}
	}
}

void drawLongLog()
{
	ICBYTES frog2, frogStill;
	ICBYTES frogBig;
	ReadImage("sprites.bmp", frog2);

	ICBYTES longLog, logs, log;
	ReadImage("longLog.bmp", logs);

	int log_x = 0;
	int log_y = 205;

	while (true)
	{
		// odunun eski hali silinsin diye
		FillRect(entryScreen, log_x + 4, log_y, 180, 40, 0x1830ac);

		Copy(logs, 1, 4, 61, 15, log);
		MagnifyX3(log, longLog);
		PasteNon0(longLog, log_x, log_y, entryScreen);

		if (frogX + 26 > log_x + 5 && frogX + 26 < log_x + 178 && frogY + 25 < log_y + 40 && frogY + 25 > log_y)
		{
			if (dir == 0)
			{
				Copy(frog2, 1, 1, 15, 15, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
			if (dir == 1)
			{
				Copy(frog2, 111, 2, 12, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
			if (dir == 2)
			{
				Copy(frog2, 74, 4, 13, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
			if (dir == 3)
			{
				Copy(frog2, 38, 2, 12, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
		}

		DisplayImage(F1, entryScreen);

		log_x += 2;
		if (frogX + 26 > log_x + 5 && frogX + 26 < log_x + 178 && frogY + 25 < log_y + 40 && frogY + 25 > log_y)
		{
			frogX += 2;
		}

		Sleep(10);

		if (log_x > 700) // ekrandan cikinca thread sonlansin
		{
			break;
		}
	}

	return;
}

void drawShortLog()
{
	ICBYTES frog2, frogStill;
	ICBYTES frogBig;
	ReadImage("sprites.bmp", frog2);

	ICBYTES shortLog, logs, log;
	ReadImage("shortLog.bmp", logs);

	int log_x = 0;
	int log_y = 250;

	while (true)
	{
		// odunun eski hali silinsin diye
		FillRect(entryScreen, log_x - 5, log_y - 5, 120, 45, 0x1830ac);

		Copy(logs, 1, 2, 30, 15, log);
		MagnifyX3(log, shortLog);
		PasteNon0(shortLog, log_x, log_y, entryScreen);

		if (frogX + 26 > log_x && frogX + 26 < log_x + 88 && frogY + 25 < log_y + 40 && frogY + 25 > log_y + 5)
		{
			if (dir == 0)
			{
				Copy(frog2, 1, 1, 15, 15, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
			if (dir == 1)
			{
				Copy(frog2, 111, 2, 12, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
			if (dir == 2)
			{
				Copy(frog2, 74, 4, 13, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
			if (dir == 3)
			{
				Copy(frog2, 38, 2, 12, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);
			}
		}

		DisplayImage(F1, entryScreen);

		log_x += 2;
		if (frogX + 26 > log_x && frogX + 26 < log_x + 88 && frogY + 25 < log_y + 40 && frogY + 25 > log_y + 5)
		{
			frogX += 2;
		}

		Sleep(28);

		if (log_x > 700) // ekrandan cikinca thread sonlansin
		{
			break;
		}
	}
}

void drawFrog()
{
	ICBYTES bottomFlower, flowers, flower;
	ICBYTES back, background;

	ICBYTES frog, frogStill;
	ICBYTES frogBig;

	ReadImage("frogger.bmp", back);
	ReadImage("sprites.bmp", flowers);

	int frog_x = 650;
	int frog_y = 298;

	int frog2_x = 700;
	int frog2_y = 298;

	int frog3_x = 750;
	int frog3_y = 298;

	while (true)
	{
		for (int i = 0; i < 10; i++)
		{
			// cicegin eski hali silinsin diye
			FillRect(entryScreen, frog_x + 8, frog_y - 5, 147, 40, 0x1830ac);

			Copy(flowers, 1, 155, 15, 10, flower);
			MagnifyX3(flower, bottomFlower);
			PasteNon0(bottomFlower, frog_x, frog_y, entryScreen);
			PasteNon0(bottomFlower, frog2_x, frog2_y, entryScreen);
			PasteNon0(bottomFlower, frog3_x, frog3_y, entryScreen);

			if (frogX + 26 > frog_x && frogX + 26 < frog_x + 150 && frogY + 25 < frog_y + 23 && frogY + 25 > frog_y)
			{
				if (dir == 0)
				{
					Copy(flowers, 1, 1, 15, 15, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 1)
				{
					Copy(flowers, 111, 2, 12, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 2)
				{
					Copy(flowers, 74, 4, 13, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 3)
				{
					Copy(flowers, 38, 2, 12, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
			}

			DisplayImage(F1, entryScreen);

			frog_x -= 2;
			frog2_x -= 2;
			frog3_x -= 2;

			if (frogX + 26 > frog_x && frogX + 26 < frog_x + 150 && frogY + 25 < frog_y + 23 && frogY + 25 > frog_y)
			{
				frogX -= 2;
			}

			Sleep(15);

		}

		for (int i = 0; i < 10; i++)
		{
			// cicegin eski hali silinsin diye
			FillRect(entryScreen, frog_x + 8, frog_y - 5, 147, 40, 0x1830ac);

			Copy(flowers, 18, 154, 16, 12, flower);
			MagnifyX3(flower, bottomFlower);
			PasteNon0(bottomFlower, frog_x, frog_y, entryScreen);
			PasteNon0(bottomFlower, frog2_x, frog2_y, entryScreen);
			PasteNon0(bottomFlower, frog3_x, frog3_y, entryScreen);

			if (frogX + 26 > frog_x && frogX + 26 < frog_x + 150 && frogY + 25 < frog_y + 23 && frogY + 25 > frog_y)
			{
				if (dir == 0)
				{
					Copy(flowers, 1, 1, 15, 15, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 1)
				{
					Copy(flowers, 111, 2, 12, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 2)
				{
					Copy(flowers, 74, 4, 13, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 3)
				{
					Copy(flowers, 38, 2, 12, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
			}

			DisplayImage(F1, entryScreen);

			frog_x -= 2;
			frog2_x -= 2;
			frog3_x -= 2;

			if (frogX + 26 > frog_x && frogX + 26 < frog_x + 150 && frogY + 25 < frog_y + 23 && frogY + 25 > frog_y)
			{
				frogX -= 2;
			}

			Sleep(15);

		}

		for (int i = 0; i < 10; i++)
		{
			// cicegin eski hali silinsin diye
			FillRect(entryScreen, frog_x + 8, frog_y - 5, 147, 40, 0x1830ac);

			Copy(flowers, 36, 154, 18, 12, flower);
			MagnifyX3(flower, bottomFlower);
			PasteNon0(bottomFlower, frog_x, frog_y, entryScreen);
			PasteNon0(bottomFlower, frog2_x, frog2_y, entryScreen);
			PasteNon0(bottomFlower, frog3_x, frog3_y, entryScreen);

			if (frogX + 26 > frog_x && frogX + 26 < frog_x + 150 && frogY + 25 < frog_y + 23 && frogY + 25 > frog_y)
			{
				if (dir == 0)
				{
					Copy(flowers, 1, 1, 15, 15, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 1)
				{
					Copy(flowers, 111, 2, 12, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 2)
				{
					Copy(flowers, 74, 4, 13, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
				if (dir == 3)
				{
					Copy(flowers, 38, 2, 12, 13, frogStill);
					MagnifyX3(frogStill, frogBig);
					PasteNon0(frogBig, frogX, frogY, entryScreen);
				}
			}

			DisplayImage(F1, entryScreen);

			frog_x -= 2;
			frog2_x -= 2;
			frog3_x -= 2;

			if (frogX + 26 > frog_x && frogX + 26 < frog_x + 150 && frogY + 25 < frog_y + 23 && frogY + 25 > frog_y)
			{
				frogX -= 2;
			}

			Sleep(15);

		}

		if (frog3_x < -55) // ekrandan cikinca thread sonlansin
		{
			break;
		}


	}
}

void collisionCheck(int x, int y, int carWidth)
{
	DWORD dw;

	FillRect(entryScreen, frogX + 26, frogY + 25, 1, 1, 0xF132D7); //kurba ic orta

	if (frogX + 26 > x && frogX + 26 < x && frogY + 25 > y + 5 && frogY + 25 < y + 42)
	{
		//// death animation
		if (!flag)
		{
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)deathAnimation, NULL, 0, &dw);

			if (life == 2)
			{
				FillRect(entryScreen, 59, 655, 40, 40, 0x000000);
			}
			else if (life == 1)
			{
				FillRect(entryScreen, 10, 655, 44, 40, 0x000000);
			}

			life--;
			flag = true;
			Restart();
		}
	}

	else if (dir == 0 && frogX + 26 > x && frogX + 26 < x + carWidth && frogY + 23 > y + 5 && frogY + 23 < y + 42)
	{
		//// death animation
		if (!flag)
		{
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)deathAnimation, NULL, 0, &dw);

			if (life == 2)
			{
				FillRect(entryScreen, 59, 655, 40, 40, 0x000000);
			}
			else if (life == 1)
			{
				FillRect(entryScreen, 10, 655, 44, 40, 0x000000);
			}

			life--;
			flag = true;
			Restart();
		}
	}

	else if (dir == 1 && frogX + 35 > x && frogX + 35 < x + carWidth && frogY + 23 > y + 5 && frogY + 23 < y + 42
		|| frogX + 20 > x && frogX + 20 < x + carWidth && frogY + 23 > y + 5 && frogY + 23 < y + 42)
	{
		// death animation
		if (!flag)
		{
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)deathAnimation, NULL, 0, &dw);

			if (life == 2)
			{
				FillRect(entryScreen, 59, 655, 40, 40, 0x000000);
			}
			else if (life == 1)
			{
				FillRect(entryScreen, 10, 655, 44, 40, 0x000000);
			}

			life--;
			flag = true;
			Restart();
		}
	}

	else if (dir == 2 && frogX + 35 > x && frogX + 35 < x + carWidth && frogY + 35 > y + 5 && frogY + 35 < y + 42
		|| frogX + 20 > x && frogX + 20 < x + carWidth && frogY + 35 > y + 5 && frogY + 35 < y + 42)
	{
		// death animation
		if (!flag)
		{
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)deathAnimation, NULL, 0, &dw);

			if (life == 2)
			{
				FillRect(entryScreen, 59, 655, 40, 40, 0x000000);
			}
			else if (life == 1)
			{
				FillRect(entryScreen, 10, 655, 44, 40, 0x000000);
			}

			life--;
			flag = true;
			Restart();
		}
	}

	else if (dir == 3 && frogX + 5 < x + 48 && frogX + 5 > x - carWidth && frogY + 23 > y + 5 && frogY + 23 < y + 42
		|| frogX + 20 < x + 20 && frogX + 20 > x - carWidth && frogY + 23 > y + 5 && frogY + 23 < y + 42)
	{
		// death animation
		if (!flag)
		{
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)deathAnimation, NULL, 0, &dw);

			if (life == 2)
			{
				FillRect(entryScreen, 59, 655, 40, 40, 0x000000);
			}
			else if (life == 1)
			{
				FillRect(entryScreen, 10, 655, 44, 40, 0x000000);
			}

			life--;
			flag = true;
			Restart();
		}
	}
}

void drawCar1() // en alttaki
{
	ICBYTES bottomCar, cars, car;
	ReadImage("sprites.bmp", cars);

	int carBottom_x = 600;
	int carBottom_y = 560;
	count1++;

	while (true)
	{
		if (count1 == 4)
		{
			count1 = 0;
			break;
		}

		// arabanin eski hali silinsin diye 
		FillRect(entryScreen, carBottom_x + 5, carBottom_y + 5, 53, 40, 0x000000);

		//// burda arabayi kopyaliyoruz
		Copy(cars, 18, 116, 19, 15, car);
		MagnifyX3(car, bottomCar);
		PasteNon0(bottomCar, carBottom_x, carBottom_y, entryScreen);

		//// burda arabayi kopyaliyoruz
		//DisplayImage(F1, entryScreen);

		Sleep(50);

		carBottom_x -= 2;

		if (carBottom_x < 10) // ekrandan cikinca thread sonlansin
		{
			break;
		}

		collisionCheck(carBottom_x, carBottom_y, 50);

		if (life == -1)
		{
			break;
		}
	}

	return;
}

void drawCar2() // en alt 2.
{
	ICBYTES bottomCar2, cars, car;
	ReadImage("sprites.bmp", cars);

	int carBottom2_x = 0;
	int carBottom2_y = 520;

	count2++;

	while (true)
	{
		if (count2 == 4)
		{
			count2 = 0;
			break;
		}

		// arabanin eski hali silinsin diye (tum ekrani yeniliyor)
		FillRect(entryScreen, carBottom2_x, carBottom2_y + 4, 45, 36, 0x000000);

		//// burda arabayi kopyaliyoruz
		Copy(cars, 55, 117, 15, 13, car);
		MagnifyX3(car, bottomCar2);
		PasteNon0(bottomCar2, carBottom2_x, carBottom2_y, entryScreen);
		//// burda arabayi kopyaliyoruz

		WaitForSingleObject(hMutex, INFINITE);
		DisplayImage(F1, entryScreen);
		ReleaseMutex(hMutex);

		Sleep(40);
		carBottom2_x += 2;

		if (carBottom2_x > 700) // ekrandan cikinca thread sonlansin
		{
			break;
		}

		collisionCheck(carBottom2_x, carBottom2_y, 15);

		if (life == -1)
		{
			break;
		}
	}
}

void drawCar3()
{
	ICBYTES bottomCar3, cars, car;
	ReadImage("sprites.bmp", cars);

	int carBottom3_x = 700;
	int carBottom3_y = 480;
	count3++;

	while (true)
	{
		if (count3 == 4)
		{
			count3 = 0;
			break;
		}

		// arabanin eski hali silinsin diye 
		FillRect(entryScreen, carBottom3_x + 3, carBottom3_y + 4, 55, 30, 0x000000);

		//// burda arabayi kopyaliyoruz
		Copy(cars, 1, 118, 17, 12, car);
		MagnifyX3(car, bottomCar3);
		PasteNon0(bottomCar3, carBottom3_x, carBottom3_y, entryScreen);
		//// burda arabayi kopyaliyoruz

		WaitForSingleObject(hMutex, INFINITE);
		DisplayImage(F1, entryScreen);
		ReleaseMutex(hMutex);

		Sleep(35);
		carBottom3_x -= 2;

		if (carBottom3_x < -50) // ekrandan cikinca thread sonlansin
		{
			break;
		}

		collisionCheck(carBottom3_x, carBottom3_y, 50);

		if (life == -1)
		{
			break;
		}
	}
}

void drawCar4()
{
	ICBYTES bottomCar2, cars, car;
	ReadImage("sprites.bmp", cars);

	int carBottom4_x = 0;
	int carBottom4_y = 430;

	while (true)
	{
		// arabanin eski hali silinsin diye (tum ekrani yeniliyor)
		FillRect(entryScreen, carBottom4_x - 5, carBottom4_y + 4, 45, 40, 0x000000);

		//// burda arabayi kopyaliyoruz
		Copy(cars, 37, 116, 16, 15, car);
		MagnifyX3(car, bottomCar2);
		PasteNon0(bottomCar2, carBottom4_x, carBottom4_y, entryScreen);
		//// burda arabayi kopyaliyoruz

		DisplayImage(F1, entryScreen);

		Sleep(40);
		carBottom4_x += 3;

		if (carBottom4_x > 700) // ekrandan cikinca thread sonlansin
		{
			break;
		}

		collisionCheck(carBottom4_x, carBottom4_y, 16);

		if (life == -1)
		{
			break;
		}
	}
}

void drawCar5()
{
	ICBYTES bottomCar5, cars, car;
	ReadImage("sprites.bmp", cars);

	int carBottom5_x = 700;
	int carBottom5_y = 385;

	count5++;

	while (true)
	{
		if (count5 == 3)
		{
			count5 = 0;
			break;
		}

		// arabanin eski hali silinsin diye 
		FillRect(entryScreen, carBottom5_x + 5, carBottom5_y + 4, 80, 36, 0x000000);

		//// burda arabayi kopyaliyoruz
		Copy(cars, 75, 118, 28, 13, car);
		MagnifyX3(car, bottomCar5);
		PasteNon0(bottomCar5, carBottom5_x, carBottom5_y, entryScreen);
		//// burda arabayi kopyaliyoruz

		DisplayImage(F1, entryScreen);

		Sleep(30);
		carBottom5_x -= 2;

		if (carBottom5_x < -150) // ekrandan cikinca thread sonlansin
		{
			break;
		}

		collisionCheck(carBottom5_x, carBottom5_y, 19);

		if (life == -1)
		{
			break;
		}
	}
}

void gamePlay(void*)
{
	ICBYTES back, background, sayi, sifir, sayiBig, smileyFrog, smiley, smileyFrogBig;
	ICBYTES time, text;

	FillRect(entryScreen, 70, 490, 560, 50, 0x000000); // deletes "press any key to start" text
	FillRect(entryScreen, 50, 200, 580, 100, 0x1830ac); // deletes "FROGGER" text

	ReadImage("sprites.bmp", frog);
	ReadImage("time.bmp", text);
	ReadImage("sprites.bmp", sayi);
	ReadImage("sprites.bmp", smiley);

	//// kurabaganin ilk yeri ve hali cizimi
	Copy(frog, 1, 1, 15, 15, frogStill);
	MagnifyX3(frogStill, frogBig);
	PasteNon0(frogBig, frogX, frogY, entryScreen);
	//// kurabaganin ilk yeri ve hali cizimi

	ReadImage("frogger.bmp", back);

	//Can cizimi
	PasteNon0(frogBig, 10, 650, entryScreen);
	PasteNon0(frogBig, 50, 650, entryScreen);

	Copy(text, 0, 4, 58, 22, time);
	PasteNon0(time, 555, 673, entryScreen);

	DisplayImage(F1, entryScreen);

	while (true)
	{
		DWORD dw;

		if (!flag)
		{
			if (keypressed == 37 && frogX - 1 > 14) //left
			{
				keypressed = NULL;

				// eski kurbagayi yok et
				Copy(back, frogX + 5, frogY, 80, 45, background);
				Paste(background, frogX + 5, frogY, entryScreen);

				Copy(frog, 55, 2, 15, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);

#ifdef _DEBUG
				Sleep(80);//DEBUG MODU YAVAÞ OLDUÐU ÝÇÝN DAHA AZ BEKLETÝYORUZ
#else
				Sleep(110); //Release mode is fast so we delay more
#endif
			// ziplayan eski kurbagayi yok et
				Copy(back, frogX + 5, frogY, 80, 45, background);
				Paste(background, frogX + 5, frogY, entryScreen);

				Copy(frog, 38, 2, 12, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX - 47, frogY, entryScreen);

				dir = 3;
				frogX -= 47;
			}

			if (keypressed == 39 && frogX + 1 < 625) //right
			{
				keypressed = NULL;

				// eski kurbagayi yok et
				Copy(back, frogX + 5, frogY, 80, 45, background);
				Paste(background, frogX + 5, frogY, entryScreen);

				Copy(frog, 126, 2, 15, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY, entryScreen);

#ifdef _DEBUG
				Sleep(80);//DEBUG MODU YAVAÞ OLDUÐU ÝÇÝN DAHA AZ BEKLETÝYORUZ
#else
				Sleep(110); //Release mode is fast so we delay more
#endif
			// ziplayan eski kurbagayi yok et
				Copy(back, frogX + 5, frogY, 80, 45, background);
				Paste(background, frogX + 5, frogY, entryScreen);

				Copy(frog, 111, 2, 12, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX + 47, frogY, entryScreen);

				dir = 1;
				frogX += 47;
			}

			if (keypressed == 38) //up
			{
				keypressed = NULL;

				if (frogY >= 593 && frogY <= 613) {
					FillRect(entryScreen, 5, 22, 60, 26, 0x000000);
					Copy(sayi, 10, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 20, 20, entryScreen);
					Copy(sayi, 1, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 40, 20, entryScreen);
				}
				else if (frogY >= 552 && frogY <= 592) {
					FillRect(entryScreen, 20, 22, 26, 26, 0x000000);
					Copy(sayi, 20, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 20, 20, entryScreen);
				}
				else if (frogY >= 500 && frogY <= 551) {
					FillRect(entryScreen, 20, 22, 26, 26, 0x000000);
					Copy(sayi, 30, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 20, 20, entryScreen);
				}
				else if (frogY >= 449 && frogY <= 499) {
					FillRect(entryScreen, 20, 22, 26, 26, 0x000000);

					Copy(sayi, 38, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 20, 20, entryScreen);
				}
				else if (frogY >= 398 && frogY <= 448) {
					FillRect(entryScreen, 20, 22, 26, 26, 0x000000);

					Copy(sayi, 48, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 20, 20, entryScreen);
				}
				else if (frogY >= 347 && frogY <= 397) {
					FillRect(entryScreen, 20, 22, 26, 26, 0x000000);

					Copy(sayi, 56, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 20, 20, entryScreen);
				}
				else if (frogY >= 297 && frogY <= 346) {
					FillRect(entryScreen, 20, 22, 26, 26, 0x000000);

					Copy(sayi, 66, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 20, 20, entryScreen);
				}
				else if (frogY >= 246 && frogY <= 296) {
					FillRect(entryScreen, 20, 22, 25, 26, 0x000000);

					Copy(sayi, 74, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 20, 20, entryScreen);
				}
				else if (frogY >= 195 && frogY <= 245) {
					FillRect(entryScreen, 20, 22, 25, 26, 0x000000);
					Copy(sayi, 84, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 20, 20, entryScreen);
				}
				else if (frogY >= 144 && frogY <= 194) {
					FillRect(entryScreen, 20, 22, 25, 26, 0x000000);
					Copy(sayi, 1, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 20, 20, entryScreen);

					Copy(sayi, 10, 333, 10, 10, sifir);
					MagnifyX3(sifir, sayiBig);
					PasteNon0(sayiBig, 2, 20, entryScreen);
				}

				// eski kurbagayi yok et
				Copy(back, frogX, frogY - 20, 45, 80, background);
				Paste(background, frogX, frogY - 20, entryScreen);

				///// ziplama animasyonu
				Copy(frog, 20, 2, 13, 14, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY - 23, entryScreen);
				///// ziplama animasyonu

#ifdef _DEBUG
				Sleep(80); // ziplamayi gormek icin sleep
#else
				Sleep(110); // release mode is fast so we delay more
#endif
			// ziplamis eski kurbagayi yok et
				Copy(back, frogX, frogY - 23, 45, 80, background);
				Paste(background, frogX, frogY - 23, entryScreen);

				//// zipladigi yerde durdugu sekli
				Copy(frog, 1, 1, 15, 15, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY - 46, entryScreen);
				//// zipladigi yerde durdugu sekli

				dir = 0;
				frogY -= 46; // yeni yeri
			}

			if (keypressed == 40 && frogY + 1 < 612) //down
			{
				keypressed = NULL;

				// eski kurbagayi yok et
				Copy(back, frogX, frogY - 20, 45, 80, background);
				Paste(background, frogX, frogY - 20, entryScreen);

				Copy(frog, 92, 1, 13, 14, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY + 23, entryScreen);

#ifdef _DEBUG
				Sleep(80);//DEBUG MODU YAVAÞ OLDUÐU ÝÇÝN DAHA AZ BEKLETÝYORUZ
#else
				Sleep(110); //Release mode is fast so we delay more
#endif
			// ziplamis eski kurbagayi yok et
				Copy(back, frogX, frogY - 23, 45, 80, background);
				Paste(background, frogX, frogY - 23, entryScreen);

				Copy(frog, 74, 4, 13, 13, frogStill);
				MagnifyX3(frogStill, frogBig);
				PasteNon0(frogBig, frogX, frogY + 46, entryScreen);

				dir = 2;
				frogY += 46;
			}
		}


		//birinci gedik için
		if (frogX + 26 >= 44 && frogX + 26 <= 97 && frogY + 25 <= 110) {
			Copy(back, frogX, frogY, 45, 80, background);
			Paste(background, frogX, frogY, entryScreen);

			Copy(smiley, 62, 195, 18, 17, smileyFrog);
			MagnifyX3(smileyFrog, smileyFrogBig);
			PasteNon0(smileyFrogBig, 44, 65, entryScreen);

			gedikCounter += 1;
			frogX = 296;
			frogY = 613;

			frogStart();

			keypressed = NULL;
			flag = false;
		}

		//ikinci gedik için
		if (frogX + 26 >= 173 && frogX + 26 <= 232 && frogY + 25 <= 110) {
			Copy(back, frogX, frogY, 45, 80, background);
			Paste(background, frogX, frogY, entryScreen);

			Copy(smiley, 62, 195, 18, 17, smileyFrog);
			MagnifyX3(smileyFrog, smileyFrogBig);
			PasteNon0(smileyFrogBig, 177, 65, entryScreen);

			gedikCounter += 1;
			frogX = 296;
			frogY = 613;

			frogStart();

			keypressed = NULL;
			flag = false;
		}

		//üçüncü gedik için
		if (frogX + 26 >= 314 && frogX + 26 <= 375 && frogY + 25 <= 110) {
			Copy(back, frogX, frogY, 45, 80, background);
			Paste(background, frogX, frogY, entryScreen);

			Copy(smiley, 62, 195, 18, 17, smileyFrog);
			MagnifyX3(smileyFrog, smileyFrogBig);
			PasteNon0(smileyFrogBig, 315, 65, entryScreen);

			gedikCounter += 1;
			frogX = 296;
			frogY = 613;

			frogStart();

			keypressed = NULL;
			flag = false;
		}

		//dördüncü gedik için
		if (frogX + 26 >= 448 && frogX + 26 <= 503 && frogY + 25 <= 110) {
			Copy(back, frogX, frogY, 45, 80, background);
			Paste(background, frogX, frogY, entryScreen);

			Copy(smiley, 62, 195, 18, 17, smileyFrog);
			MagnifyX3(smileyFrog, smileyFrogBig);
			PasteNon0(smileyFrogBig, 449, 65, entryScreen);

			gedikCounter += 1;
			frogX = 296;
			frogY = 613;

			frogStart();

			keypressed = NULL;
			flag = false;
		}

		//beþinci gedik için
		if (frogX + 26 >= 580 && frogX + 26 <= 641 && frogY + 25 <= 110) {
			Copy(back, frogX, frogY, 45, 80, background);
			Paste(background, frogX, frogY, entryScreen);

			Copy(smiley, 62, 195, 18, 17, smileyFrog);
			MagnifyX3(smileyFrog, smileyFrogBig);
			PasteNon0(smileyFrogBig, 581, 65, entryScreen);

			gedikCounter += 1;
			frogX = 296;
			frogY = 613;

			frogStart();

			keypressed = NULL;
			flag = false;

		}

		Win();


		if (life == -1)
		{
			break;
		}
	}
}

void ICGUI_main()
{
	DWORD dw;

	F1 = ICG_FrameThin(0, 0, 900, 657);
	SLE = ICG_SLEditBorder(1100, 13, 90, 50, "bisiler");

	ICG_SetOnKeyPressed(WhenKeyPressed);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startGame, NULL, 0, &dw); // start screen
}
