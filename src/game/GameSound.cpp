/****************************************************************************
 * Copyright (C) 2015 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include "GameSound.h"
#include "sounds/SoundHandler.hpp"

GameSound::GameSound(const char * filepath)
{
	voice = -1;
	Load(filepath);
}

GameSound::GameSound(const u8 * snd, int length)
{
	voice = -1;
	Load(snd, length);
}

GameSound::~GameSound()
{
	if(voice >= 0)
	{
		SoundHandler::instance()->RemoveDecoder(voice);
	}
}


bool GameSound::Load(const char * filepath)
{
	if(voice >= 0)
	{
		SoundHandler::instance()->RemoveDecoder(voice);
		voice = -1;
	}

	//! find next free decoder
	for(int i = 0; i < MAX_DECODERS; i++)
	{
		SoundDecoder * decoder = SoundHandler::instance()->getDecoder(i);
		if(decoder == NULL)
		{
			SoundHandler::instance()->AddDecoder(i, filepath);
			decoder = SoundHandler::instance()->getDecoder(i);
			if(decoder)
			{
				voice = i;
				SoundHandler::instance()->ThreadSignal();
			}
			break;
		}
	}

	if(voice < 0) {
		return false;
	}

	return true;
}

bool GameSound::Load(const u8 * snd, s32 len)
{
	if(voice >= 0)
	{
		SoundHandler::instance()->RemoveDecoder(voice);
		voice = -1;
	}

	if(!snd)
		return false;

	//! find next free decoder
	for(int i = 0; i < MAX_DECODERS; i++)
	{
		SoundDecoder * decoder = SoundHandler::instance()->getDecoder(i);
		if(decoder == NULL)
		{
			SoundHandler::instance()->AddDecoder(i, snd, len);
			decoder = SoundHandler::instance()->getDecoder(i);
			if(decoder)
			{
				voice = i;
				SoundHandler::instance()->ThreadSignal();
			}
			break;
		}
	}

	if(voice < 0) {
		return false;
	}

	return true;
}

void GameSound::Play()
{
	Stop();

	Voice * v = SoundHandler::instance()->getVoice(voice);
	if(v)
		v->setState(Voice::STATE_START);


}

void GameSound::Stop()
{
	Voice * v = SoundHandler::instance()->getVoice(voice);
	if(v)
	{
		if((v->getState() != Voice::STATE_STOP) && (v->getState() != Voice::STATE_STOPPED))
			v->setState(Voice::STATE_STOP);

		while(v->getState() != Voice::STATE_STOPPED)
			usleep(1000);
	}

	SoundDecoder * decoder = SoundHandler::instance()->getDecoder(voice);
	if(decoder)
	{
		decoder->Lock();
		decoder->Rewind();
		decoder->ClearBuffer();
		SoundHandler::instance()->ThreadSignal();
		decoder->Unlock();
	}
}

void GameSound::Pause()
{
	if(!IsPlaying())
		return;

	Voice * v = SoundHandler::instance()->getVoice(voice);
	if(v)
		v->setState(Voice::STATE_STOP);
}

void GameSound::Resume()
{
	if(IsPlaying())
		return;

	Voice * v = SoundHandler::instance()->getVoice(voice);
	if(v)
		v->setState(Voice::STATE_START);
}

bool GameSound::IsPlaying()
{
	Voice * v = SoundHandler::instance()->getVoice(voice);
	if(v) {
		return v->getState() == Voice::STATE_PLAYING;
	}

	return false;

}

void GameSound::SetVolume(u32 vol)
{
	if(vol > 100)
		vol = 100;

	u32 volumeConv = ( (0x8000 * vol) / 100 ) << 16;

	Voice * v = SoundHandler::instance()->getVoice(voice);
	if(v)
		v->setVolume(volumeConv);
}

void GameSound::SetLoop(bool l)
{
	SoundDecoder * decoder = SoundHandler::instance()->getDecoder(voice);
	if(decoder)
		decoder->SetLoop(l);
}

void GameSound::Rewind()
{
	Stop();
}

char * GameSound::getTrackName()
{
	return SoundHandler::instance()->getDecoder(voice)->getTrackName();
}

char * GameSound::getArtistName()
{
	return SoundHandler::instance()->getDecoder(voice)->getArtistName();
}
