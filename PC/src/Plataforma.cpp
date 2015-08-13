#include "Plataforma.h"

Plataforma::Plataforma(void)
{
	this->Initialize();
}

Plataforma::~Plataforma(void)
{
	this->Dispose();
}

void Plataforma::Initialize()
{
	TextureMgr *texMgr = TextureMgr::GetInstance();

	_frames.push_back(Frame("data/TilePlat0.png"));
	_frames.push_back(Frame("data/TilePlat1.png"));
	_frames.push_back(Frame("data/TilePlat2.png"));
	_frames.push_back(Frame("data/TilePlat3.png"));
	_frames.push_back(Frame("data/TilePlat4.png"));
	_frames.push_back(Frame("data/TilePlat5.png"));
	_frames.push_back(Frame("data/TilePlat6.png"));
	_frames.push_back(Frame("data/TilePlat7.png"));

	this->_currentFrame = _frames.begin();

	this->_disposed = false;
	this->_g = Graphics::GetInstance();
}

bool Plataforma::DrawWhenNoCoins()
{
	return true;
}

void Plataforma::Dispose()
{
	if(!this->_disposed)
	{
		this->_disposed = true;
	}
}

void Plataforma::setTipoPlataforma(TilePlataforma tipo)
{
	this->_tipoPlataforma = tipo;
	this->_currentFrame = this->_frames.begin() + tipo;
}

void Plataforma::setTileSize(VECTOR2 tileSize)
{
	this->_tileSize = tileSize;
	updateDrawingCoords();
}

void Plataforma::setPos(char tileX, char tileY)
{
	this->_x = tileX;
	this->_y = tileY;
	updateDrawingCoords();
}

void Plataforma::setLongitud(char longitud)
{
	this->_longitud = longitud;
	updateDrawingCoords();
}

void Plataforma::setDireccion(Direccion dir)
{
	this->_direccion = dir;
	updateDrawingCoords();
}

void Plataforma::DrawShadow()
{
	Frame &current = *this->_currentFrame;
	_g->BlitShadow(current, this->_iniX, this->_iniY, this->_finX, this->_finY, false, false);
}

void Plataforma::Draw()
{
	Frame &current = *this->_currentFrame;
	_g->BlitFrame(current, _iniX, _iniY, _finX, _finY, false, false);
}

void Plataforma::updateDrawingCoords()
{
	Frame &current = *this->_currentFrame;
	_iniX = this->_x * this->_tileSize.x;
	_iniY = this->_y * this->_tileSize.y;
	_finX = (this->_direccion == Horizontal) ? (this->_longitud * this->_tileSize.x) : this->_tileSize.x;
	_finY = (this->_direccion == Vertical) ? (this->_longitud * this->_tileSize.y) : this->_tileSize.y;
	current.Coords.tx2 = (this->_direccion == Horizontal) ? (float) this->_longitud : 1.0f ;
	current.Coords.ty2 = (this->_direccion == Vertical) ? (float) this->_longitud : 1.0f ;
}
