#pragma once

#include "TextureMgr.h"
#include "InputManager.h"
#include "Graphics.h"
#include "GameData.h"
#include "IGameState.h"
#include "Scanlines.h"
#include "Stage.h"
#include "Presentacion.h"
#include "Portada.h"
#include <iostream>
#include <iterator>
#include <algorithm>

class Game
{
public:
	static Game* GetInstance();

	bool Initialize(int width, int height, bool fullscreen);
	void SetWindowName(char *name);
	void Update(Uint32 mSecs);
	void Render();
	void Dispose();
	bool Running();
	void ShowCursor(bool show);

	void SetAttractMode(bool attract);
	void SetSaveAttract(bool save);

	void LoadAttractModeData();
	void SaveAttractModeData();

	void AddState(IGameState* state);
protected:
	Game(void);
	~Game(void);

	static Game _instance;

private:
	bool _running;

	bool _attractMode;
	bool _savingStatus;
	bool _statusSaved;
	Uint32 _totalTicks;
	vector<Uint32> _eventBuffer;
	vector<Uint32>::iterator _evtBufferIterator;

	Graphics *_g;
	InputManager *_input;
	TextureMgr *_texMgr;
	SDL_Surface *_screen;
	Scanlines *_scanlines;

	map<string, IGameState*> _states;
	map<string, IGameState*>::iterator _statesIt;
	string _currentStatus;

	void loadResources();
};
