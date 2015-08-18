#include "Player.h"

TPlayer::TPlayer()
{
	this->_graphics = Graphics::GetInstance();
	this->_animPlayer.setAnimation(Animation::Get("uwol_stand_right"));
	this->_input = InputManager::GetInstance();
	this->_map = NULL;

	this->_colRect.x = 4;
	this->_colRect.y = 4;
	this->_colRect.width = 24;
	this->_colRect.height = 28;

	this->_posRect.x = 0;
	this->_posRect.y = 0;
	this->_posRect.width = 32;
	this->_posRect.height = 32;

	this->Initialize();

	this->_disposed = false;
}

TPlayer::~TPlayer()
{
	this->Dispose();
}

void TPlayer::Initialize()
{
	this->initializePlayerData();
	this->_vidas = 3;
	this->_score = 0;
	this->_coinsTaken = 0;
}

void TPlayer::initializePlayerData()
{
	this->_x = 32;
	this->_y = 256;
	this->_vy = 0;
	this->_g = 0.3f;
	this->_vx = 0;
	this->_ax = 0.4f;
	this->_rx = 0.15f;
	this->_salto = 0.75f;
	this->_contSalto = 1;
	this->_saltando = false;
	this->_frame = 0;
	this->_facing = Right;
	this->_estado = Normal;
	this->_alpha = 1.0f;
}

void TPlayer::Dispose()
{
	if (!this->_disposed)
	{
		this->_disposed = true;
	}
}

void TPlayer::setMap(CollisionMap *map)
{
	this->_map = map;
}

bool TPlayer::DrawWhenNoCoins()
{
	return true;
}

bool TPlayer::UpdateWhenNoCoins()
{
	return true;
}

void TPlayer::Draw()
{
	if (!(this->_estado & Parpadeo) || this->_visibleParpadeo) {
		_graphics->BlitFrameAlpha(_animPlayer.GetCurrentFrame(),
			(int)this->_x, (int)this->_y, this->_tileSize.x, this->_tileSize.y,
			this->_alpha, this->_facing != Right, false);
	}
}

void TPlayer::DrawShadow()
{
	if (!(this->_estado & Parpadeo) || this->_visibleParpadeo) {
		_graphics->BlitShadow(_animPlayer.GetCurrentFrame(),
			(int)this->_x, (int)this->_y, this->_tileSize.x, this->_tileSize.y,
			this->_facing != Right, false);
	}
}

void TPlayer::Update(Uint32 milliSec)
{
	if (!(this->_estado & Muriendo))
	{
		if (this->_estado & Parpadeo)
		{
			this->_contParpadeo += milliSec;
			this->_visibleParpadeo = (this->_contParpadeo / TICKS_PARPADEO) & 1;
			if (this->_contParpadeo > TICKS_INVULNERABLE)
			{
				this->_estado = this->_estado & ~(Parpadeo);
			}
		}

		this->checkInput(milliSec);
		this->checkMapCollisions(milliSec);

		if (this->_vx == 0.0f) {
			this->setAnimation("uwol_stand_right");
		}
		else {
			this->setAnimation("uwol_walk_right");
		}

		if (this->_y > 288.0f)
		{
			// UWOL se ha ca�do, y debe morir como un perro... ;)
			this->setEstado(Muriendo);
		}
	}
	else
	{
		MusicManager *musMgr = MusicManager::GetInstance();

		this->_vy += this->_g * milliSec * PIX_PER_MILLISEC;
		this->_y += this->_vy * milliSec * PIX_PER_MILLISEC;

		if (this->_y > 320.0f && !musMgr->IsPlayingMusic())
		{
			this->_vidas--;
			this->setEstado(Muerto);
		}
	}

	this->_posRect.x = this->_x + this->_colRect.x;
	this->_posRect.y = this->_y + this->_colRect.y;
	this->_posRect.width = this->_colRect.width;
	this->_posRect.height = this->_colRect.height;

	vector<Event> evts = this->_animPlayer.Update(milliSec);
	for (Event e : evts) {
		if (e.Name == "uwol_step" && this->_vy != 0) {
			if (this->_lastStep == 0) {
				MusicManager::PlayFx("sounds/StepStone1.ogg", false);
			}
			else {
				MusicManager::PlayFx("sounds/StepStone2.ogg", false);
			}
		}
	}
}

bool TPlayer::Collides(int tileX, int tileY)
{
	if (tileX > this->_map->cols - 1)
	{
		tileX = 0;
	}
	if (tileX < 0)
	{
		tileX = this->_map->cols - 1;
	}

	if (tileY < 0)
		return false;

	if (tileY > this->_map->rows) {
		return false;
	}

	return (this->_map->map[tileX + (tileY * this->_map->cols)] == COLLISION_BLOCK);
}

void TPlayer::getTiles(int &tileX, int &tileY, int &tileX2, int &tileY2)
{
	if ((this->_x + this->_colRect.x) / this->_map->cellWidth >= 0.0f)
	{
		tileX = (int)((this->_x + this->_colRect.x) / this->_map->cellWidth);
	}
	else
	{
		tileX = -1;
	}

	if ((this->_y + this->_colRect.y) / this->_map->cellHeight >= 0.0f)
	{
		tileY = (int)((this->_y + _colRect.y) / this->_map->cellHeight);
	}
	else
	{
		tileY = -1;
	}

	if ((this->_x + this->_colRect.x + this->_colRect.width - 1) / this->_map->cellWidth >= 0.0f)
	{
		tileX2 = (int)((this->_x + this->_colRect.x + this->_colRect.width - 1) / this->_map->cellWidth);
	}
	else
	{
		tileX2 = -1;
	}

	if ((this->_y + this->_colRect.y + this->_colRect.height - 1) / this->_map->cellHeight >= 0.0f)
	{
		tileY2 = (int)((this->_y + _colRect.y + this->_colRect.height - 1) / this->_map->cellHeight);
	}
	else
	{
		tileY2 = -1;
	}
}

void TPlayer::checkMapCollisions(Uint32 milliSec)
{
	int tileX, tileY, tileX2, tileY2;
	float dx, dy;

	dy = this->_vy * milliSec * PIX_PER_MILLISEC;
	this->_y += dy;

	this->getTiles(tileX, tileY, tileX2, tileY2);

	if (this->_vy < 0.0f)
	{
		// Subiendo:
		if (this->Collides(tileX, tileY) || this->Collides(tileX2, tileY))
		{
			this->_y = (float)((tileY + 1) * this->_map->cellHeight) - this->_colRect.y;
			this->_vy *= -0.01f;
			this->_saltando = false;
		}
	}
	else if (this->_vy > 0.0f)
	{
		// Bajando:
		if (this->Collides(tileX, tileY2) || this->Collides(tileX2, tileY2))
		{
			this->_y = (float)(tileY2 * this->_map->cellHeight) - (this->_colRect.y + this->_colRect.height);
			this->_vy = 0.0f;
		}
	}

	dx = this->_vx * milliSec * PIX_PER_MILLISEC;
	this->_x += dx;

	this->getTiles(tileX, tileY, tileX2, tileY2);

	if (this->_vx < 0.0f)
	{
		if (this->Collides(tileX, tileY) || this->Collides(tileX, tileY2))
		{
			this->_vx = 0.0f;
			this->_x = (float)((tileX + 1) * this->_map->cellWidth) - this->_colRect.x;
		}
	}
	else if (this->_vx > 0.0f)
	{
		if (this->Collides(tileX2, tileY) || this->Collides(tileX2, tileY2))
		{
			this->_vx = 0.0f;
			this->_x = (float)(tileX2 * this->_map->cellWidth) - (this->_colRect.x + this->_colRect.width);
		}
	}

	// Wrap around...
	float minX = (float)-(this->_colRect.width - 1);
	float maxX = (float)(_graphics->WorldWidth - 1);
	if (this->_x < minX) {
		this->_x = maxX;
	}
	if (this->_x > maxX) {
		this->_x = minX;
	}
}

void TPlayer::DebugPaint()
{
	vector<VECTOR2> vertexes;

	int x1 = (int)(this->_x + this->_colRect.x);
	int x2 = (int)(x1 + this->_colRect.width);
	int y1 = (int)(this->_y + this->_colRect.y);
	int y2 = (int)(y1 + this->_colRect.height);

	vertexes.push_back(VECTOR2(x1, y1));
	vertexes.push_back(VECTOR2(x2, y1));
	vertexes.push_back(VECTOR2(x2, y2));
	vertexes.push_back(VECTOR2(x1, y2));
	vertexes.push_back(VECTOR2(x1, y1));

	_graphics->DrawPolyLines(vertexes, 1.0f, 1.0f, 1.0f, 1.0f);
}

void TPlayer::checkInput(Uint32 milliSec)
{
	bool horizKeyPressed = false;

	if (_input->IsKeyPressed(ActionKeysDown) && this->_estado & TodasMonedasCogidas)
	{
		int tileX1, tileX2, tileY1, tileY2;

		int posMap1, posMap2, posMap3, posMap4;

		posMap1 = this->_map->cols * 9 + 1;
		posMap2 = this->_map->cols * 8 + 1;
		posMap3 = this->_map->cols * 9 + 10;
		posMap4 = this->_map->cols * 8 + 10;

		tileX1 = (int)((this->_x + this->_colRect.x) / this->_map->cellWidth);
		tileX2 = (int)((this->_x + this->_colRect.x + this->_colRect.width - 1) / this->_map->cellWidth);
		tileY1 = (int)((this->_y + this->_colRect.y + this->_colRect.height - 1) / this->_map->cellHeight);
		tileY2 = (int)((this->_y + this->_colRect.y + this->_colRect.height) / this->_map->cellHeight);
		// Comprobamos si est� encima de una salida...
		if (this->_map->map[posMap1] == COLLISION_BLOCK && this->_map->map[posMap2] != COLLISION_BLOCK && (tileX1 == 1 || tileX2 == 1) && (tileY1 == 8 && tileY2 == 9))
		{
			this->_estado |= SalidaIzq;
		}
		if (this->_map->map[posMap1] == COLLISION_BLOCK && this->_map->map[posMap2] != COLLISION_BLOCK && (tileX1 == 10 || tileX2 == 10) && (tileY1 == 8 && tileY2 == 9))
		{
			this->_estado |= SalidaDer;
		}
	}

	if (_input->IsKeyPressed(ActionKeysUp))
	{
		if (!this->_saltando && this->_vy == 0.0f)
		{
			this->_saltando = true;
			this->_contSalto = 0;
			MusicManager::PlayFx("sounds/boing.ogg", false);
		}

		if (this->_saltando && this->_contSalto < TICKS_SALTO)
		{
			this->_contSalto += milliSec;
			if (this->_contSalto > TICKS_SALTO)
			{
				this->_vy += this->_salto * (milliSec - (this->_contSalto - TICKS_SALTO)) * PIX_PER_MILLISEC;
				this->_saltando = false;
			}
			else
			{
				this->_vy -= this->_salto * milliSec * PIX_PER_MILLISEC;
			}
		}
	}
	else
	{
		this->_saltando = false;
	}

	if (_input->IsKeyPressed(ActionKeysLeft))
	{
		if (this->_vx > -3.0f)
		{
			this->_vx -= this->_ax * milliSec * PIX_PER_MILLISEC;
			if (this->_facing == Right && this->_vx < 0)
			{
				this->_facing = Left;
			}
		}
		horizKeyPressed = true;
	}

	if (_input->IsKeyPressed(ActionKeysRight))
	{
		if (this->_vx < 3.0f)
		{
			this->_vx += this->_ax * milliSec * PIX_PER_MILLISEC;
			if (this->_facing == Left && this->_vx > 0)
			{
				this->_facing = Right;
			}
		}
		horizKeyPressed = true;
	}

	if (!horizKeyPressed)
	{
		if (this->_vx > 0)
		{
			this->_vx -= this->_rx * milliSec * PIX_PER_MILLISEC;
			if (this->_vx < 0)
			{
				this->_vx = 0;
			}
		}
		else
		{
			if (this->_vx < 0)
			{
				this->_vx += this->_rx * milliSec * PIX_PER_MILLISEC;
				if (this->_vx > 0)
				{
					this->_vx = 0;
				}
			}
		}
	}

	this->_vy += this->_g * milliSec * PIX_PER_MILLISEC;

	if (this->_vy < -32.0f)
	{
		this->_vy = -32.0f;
	}

	if (this->_vy > 32.0f)
	{
		this->_vy = 32.0f;
	}
}

void TPlayer::setEstado(int estado)
{
	this->_estado = estado;

	if (estado & Parpadeo)
	{
		_visibleParpadeo = true;
		_contParpadeo = 0;

		if (this->_facing == Left)
		{
			this->_vx = 5.0f;
		}
		else
		{
			this->_vx = -5.0f;
		}

		this->_vy = -7.5f;
		this->_saltando = false;
	}

	if (estado & Muriendo)
	{
		this->setAnimation("uwol_die");
		MusicManager::PlayFx("sounds/DeathCry.ogg", false);
		MusicManager::PlayMusic("music/Death.ogg", false);
		this->_vy = -10.0f;
	}
}

int TPlayer::getEstado()
{
	return this->_estado;
}

void TPlayer::setAlpha(float alpha)
{
	this->_alpha = alpha;
}

void TPlayer::setPosition(char tileX, char tileY)
{
	this->_x = (float)(tileX * this->_tileSize.x);
	this->_y = (float)(tileY * this->_tileSize.y);
}

void TPlayer::setTileSize(VECTOR2 tileSize)
{
	this->_tileSize = tileSize;
}

void TPlayer::setAnimation(string name) {
	stringstream ss;
	ss << name;
	if (this->_estado & Desnudo) {
		ss << "_naked";
	}
	this->_animPlayer.setAnimation(Animation::Get(ss.str()));
}