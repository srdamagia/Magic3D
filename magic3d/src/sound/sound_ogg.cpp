/******************************************************************************
 @Copyright    Copyright (C) 2008 - 2015 by MagicTech.

 @Platform     ANSI compatible
******************************************************************************/
/*
Magic3D Engine
Copyright (c) 2008-2015
Thiago C. Moraes
http://www.magictech.com.br

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include <magic3d/magic3d.h>
#include <magic3d/sound/sound_ogg.h>

Magic3D::SoundOGG::SoundOGG(const SoundOGG& sound, std::string name) : Sound(sound, name)
{
    this->psVorbisInfo = NULL;
    this->vorbisComment = NULL;
    this->pDecodeBuffer = NULL;

    this->ulFrequency = sound.ulFrequency;
    this->ulFormat = sound.ulFormat;
    this->ulChannels = sound.ulChannels;
    this->ulBufferSize = sound.ulBufferSize;
    this->ulBytesWritten = sound.ulBytesWritten;

    this->uiSource = sound.uiSource;
    this->uiBuffer = sound.uiBuffer;
    this->iState = sound.iState;
    this->iLoop = sound.iLoop;
    this->iBuffersProcessed = sound.iBuffersProcessed;
    this->iTotalBuffersProcessed = sound.iTotalBuffersProcessed;
    this->iQueuedBuffers = sound.iQueuedBuffers;

    this->loaded = false;
    this->started = false;

    memcpy(this->uiBuffers, sound.uiBuffers, sizeof(this->uiBuffers));

    if (sound.loaded)
    {
        this->load();

        if (this->loaded && sound.started)
        {
            this->play();
        }
    }
}

Magic3D::SoundOGG::SoundOGG(std::string name) : Sound(name)
{
    memset(uiBuffers, 0, sizeof(uiBuffers));
    uiSource               = 0;
    uiBuffer               = 0;
    iState                 = 0;
    iLoop                  = 0;
    iBuffersProcessed      = 0;
    iTotalBuffersProcessed = 0;
    iQueuedBuffers         = 0;

    ulFrequency            = 0l;
    ulFormat               = 0l;
    ulChannels             = 0l;
    ulBufferSize           = 0l;
    ulBytesWritten         = 0l;

    pDecodeBuffer          = NULL;
    psVorbisInfo           = NULL;
    vorbisComment          = NULL;

    loaded                 = false;
    started                = false;
}

Magic3D::SoundOGG::~SoundOGG()
{
    release();
}

Magic3D::SoundOGG* Magic3D::SoundOGG::spawn(std::string name) const
{
    return (new SoundOGG(*this, name));
}

void Magic3D::SoundOGG::load()
{
    loaded = false;
    started = false;
    Log::logFormat(eLOG_PLAINTEXT, "Start Loading Sound: %s", getFileName().c_str());

    std::string path = ResourceManager::getInstance()->getPath() + M3D_PATH_SOUND + getFileName();
    // Open the OggVorbis file
    FILE* pOggVorbisFile = fopen(path.c_str(), "rb");
    if (pOggVorbisFile)
    {
        // Create an OggVorbis file stream
        if (ov_open(pOggVorbisFile, &sOggVorbisFile, NULL, 0) == 0)
        {
            // Get some information about the file (Channels, Format, and Frequency)
            psVorbisInfo = ov_info(&sOggVorbisFile, -1);
            vorbisComment = ov_comment(&sOggVorbisFile, -1);
            if (psVorbisInfo)
            {
                ulFrequency = psVorbisInfo->rate;
                ulChannels = psVorbisInfo->channels;

                switch (psVorbisInfo->channels)
                {
                    case 1:
                    {
                        ulFormat = AL_FORMAT_MONO16;
                        // Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
                        ulBufferSize = ulFrequency >> 1;
                        // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
                        ulBufferSize -= (ulBufferSize % 2);
                        break;
                    }
                    case 2:
                    {
                        ulFormat = AL_FORMAT_STEREO16;
                        // Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
                        ulBufferSize = ulFrequency;
                        // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
                        ulBufferSize -= (ulBufferSize % 4);
                        break;
                    }
                    case 4:
                    {
                        ulFormat = alGetEnumValue("AL_FORMAT_QUAD16");
                        // Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
                        ulBufferSize = ulFrequency * 2;
                        // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
                        ulBufferSize -= (ulBufferSize % 8);
                        break;
                    }
                    case 6:
                    {
                        ulFormat = alGetEnumValue("AL_FORMAT_51CHN16");
                        // Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
                        ulBufferSize = ulFrequency * 3;
                        // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
                        ulBufferSize -= (ulBufferSize % 12);
                        break;
                    }
                }
            }

            if (ulFormat != 0)
            {
                // Allocate a buffer to be used to store decoded data for all Buffers
                //ulBufferSize = 4096 * 8;
                pDecodeBuffer = new char[ulBufferSize];
                if (pDecodeBuffer)
                {
                    // Generate some AL Buffers for streaming
                    alGenBuffers( M3D_SOUND_NUMBUFFERS, uiBuffers );
                    // Generate a Source to playback the Buffers
                    alGenSources( 1, &uiSource );

                    alDistanceModel(AL_LINEAR_DISTANCE);
                    alSource3f(uiSource, AL_POSITION,           0.0f, 0.0f, 0.0f);
                    alSource3f(uiSource, AL_VELOCITY,           0.0f, 0.0f, 0.0f);
                    alSource3f(uiSource, AL_DIRECTION,          0.0f, 0.0f, 0.0f);
                    alSourcef (uiSource, AL_ROLLOFF_FACTOR,     1.0f);
                    alSourcef (uiSource, AL_MAX_DISTANCE,       getDistance());
                    alSourcef (uiSource, AL_REFERENCE_DISTANCE, 1.0f);
                    alSourcef (uiSource, AL_PITCH,              getPitch());
                    alSourcef (uiSource, AL_GAIN,               getGain());

                    Log::logFormat(eLOG_RENDERER, "Version:         %d", psVorbisInfo->version);
                    Log::logFormat(eLOG_RENDERER, "Channels:        %d", psVorbisInfo->channels);
                    Log::logFormat(eLOG_RENDERER, "Rate (hz):       %d", psVorbisInfo->rate);
                    Log::logFormat(eLOG_RENDERER, "Bitrate upper:   %d", psVorbisInfo->bitrate_upper);
                    Log::logFormat(eLOG_RENDERER, "Bitrate nominal: %d", psVorbisInfo->bitrate_nominal);
                    Log::logFormat(eLOG_RENDERER, "Bitrate lower:   %d", psVorbisInfo->bitrate_lower);
                    Log::logFormat(eLOG_RENDERER, "Bitrate window:  %d", psVorbisInfo->bitrate_window);
                    Log::logFormat(eLOG_RENDERER, "Vendor:          %s", vorbisComment->vendor);
                    for(int i = 0; i < vorbisComment->comments; i++) {
                        Log::log(eLOG_RENDERER, vorbisComment->user_comments[i]);
                    }
                    loaded = true;
                }
                else
                {
                    Log::log(eLOG_FAILURE, "Failed to allocate memory for decoded OggVorbis data");
                    ov_clear(&sOggVorbisFile);
                }
            }
            else
            {
                Log::log(eLOG_FAILURE, "Failed to find format information, or unsupported format");
            }
        }
        else
        {
            Log::logFormat(eLOG_FAILURE, "Could not open: %s", path.c_str());
        }
    }
    else
    {
        Log::logFormat(eLOG_FAILURE, "Could not find: %s", path.c_str());
    }
    Log::logFormat(eLOG_SUCCESS, "Finish Loading Sound: %s", getFileName().c_str());
}

void Magic3D::SoundOGG::release()
{
    // Stop the Source and clear the Queue
    stop();

    alSourcei(uiSource, AL_BUFFER, 0);
    // Clean up buffers and sources    
    alDeleteBuffers( M3D_SOUND_NUMBUFFERS, uiBuffers );
    alDeleteSources( 1, &uiSource );

    if (pDecodeBuffer)
    {
        delete[] pDecodeBuffer;
    }    

    // Close OggVorbis stream
    ov_clear(&sOggVorbisFile);
    //if (pOggVorbisFile) {
    //    fclose(pOggVorbisFile);
    //}
}

void Magic3D::SoundOGG::play()
{
    if (loaded)
    {
        if (isPlaying())
        {
            stop();
        }
        ov_time_seek(&sOggVorbisFile, 0.0);
        // Fill all the Buffers with decoded audio data from the OggVorbis file
        for (iLoop = 0; iLoop < M3D_SOUND_NUMBUFFERS; iLoop++)
        {
            ulBytesWritten = decode(&sOggVorbisFile, pDecodeBuffer, ulBufferSize, ulChannels);
            if (ulBytesWritten)
            {
                alBufferData(uiBuffers[iLoop], ulFormat, pDecodeBuffer, ulBytesWritten, ulFrequency);
                alSourceQueueBuffers(uiSource, 1, &uiBuffers[iLoop]);
            }
        }

        // Start playing source
        alSourcePlay(uiSource);
        iTotalBuffersProcessed = 0;
        started = true;
    }
}

void Magic3D::SoundOGG::stop()
{
    if (isPlaying())
    {
        alSourceStop(uiSource);        
        for (iLoop = 0; iLoop < M3D_SOUND_NUMBUFFERS; iLoop++)
        {
            uiBuffer = 0;
            alSourceUnqueueBuffers(uiSource, 1, &uiBuffer);
        }
    }
    started = false;
}

bool Magic3D::SoundOGG::isPlaying()
{
    alGetSourcei(uiSource, AL_SOURCE_STATE, &iState);
    return iState == AL_PLAYING;
}

void Magic3D::SoundOGG::render()
{
    if (loaded && started)
    {
        Matrix4 m = getMatrixFromParent();
        alSource3f(uiSource, AL_POSITION,           m.getTranslation().getX(), m.getTranslation().getY(), m.getTranslation().getZ());
        alSource3f(uiSource, AL_VELOCITY,           0.0f, 0.0f, 0.0f);
        alSource3f(uiSource, AL_DIRECTION,          0.0f, 0.0f, 0.0f);
        alSourcef (uiSource, AL_ROLLOFF_FACTOR,     1.0f);
        alSourcef (uiSource, AL_MAX_DISTANCE,       getDistance());
        alSourcef (uiSource, AL_REFERENCE_DISTANCE, 1.0f);
        alSourcef (uiSource, AL_PITCH,              getPitch());
        alSourcef (uiSource, AL_GAIN,               getGain());

        // Request the number of OpenAL Buffers have been processed (played) on the Source
        iBuffersProcessed = 0;
        alGetSourcei(uiSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

        // Keep a running count of number of buffers processed (for logging purposes only)
        iTotalBuffersProcessed += iBuffersProcessed;

        // For each processed buffer, remove it from the Source Queue, read next chunk of audio
        // data from disk, fill buffer with new data, and add it to the Source Queue
        while (iBuffersProcessed)
        {
            // Remove the Buffer from the Queue.  (uiBuffer contains the Buffer ID for the unqueued Buffer)
            uiBuffer = 0;
            alSourceUnqueueBuffers(uiSource, 1, &uiBuffer);

            // Read more audio data (if there is any)
            ulBytesWritten = decode(&sOggVorbisFile, pDecodeBuffer, ulBufferSize, ulChannels);
            if (ulBytesWritten)
            {
                alBufferData(uiBuffer, ulFormat, pDecodeBuffer, ulBytesWritten, ulFrequency);
                alSourceQueueBuffers(uiSource, 1, &uiBuffer);
            }
            iBuffersProcessed--;
        }

        // Check the status of the Source.  If it is not playing, then playback was completed,
        // or the Source was starved of audio data, and needs to be restarted.
        if (!isPlaying())
        {
            // If there are Buffers in the Source Queue then the Source was starved of audio
            // data, so needs to be restarted (because there is more audio data to play)
            alGetSourcei(uiSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);

            if (iQueuedBuffers)
            {
                alSourcePlay(uiSource);
            }
            else
            {
                // Finished playing
                if (started)
                {
                    ov_time_seek(&sOggVorbisFile, 0.0);
                    if (!isLooping())
                    {
                        stop();
                    }
                    else
                    {
                        play();
                    }
                }
            }
        }
    }
}

unsigned long Magic3D::SoundOGG::decode(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels) {
    int current_section;
    long lDecodeSize;
    unsigned long ulSamples;
    short *pSamples;

    unsigned long ulBytesDone = 0;
    while (true)
    {
        lDecodeSize = ov_read(psOggVorbisFile, pDecodeBuffer + ulBytesDone, ulBufferSize - ulBytesDone, 0, 2, 1, &current_section);
        if (lDecodeSize > 0)
        {
            ulBytesDone += lDecodeSize;

            if (ulBytesDone >= ulBufferSize)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    // Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
    // however 6-Channels files need to be re-ordered
    if (ulChannels == 6)
    {
        pSamples = (short*)pDecodeBuffer;
        for (ulSamples = 0; ulSamples < (ulBufferSize >> 1); ulSamples += 6)
        {
            // WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
            // OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
            swap(pSamples[ulSamples+1], pSamples[ulSamples+2]);
            swap(pSamples[ulSamples+3], pSamples[ulSamples+5]);
            swap(pSamples[ulSamples+4], pSamples[ulSamples+5]);
        }
    }

    return ulBytesDone;
}

void Magic3D::SoundOGG::swap(short &s1, short &s2)
{
    short sTemp = s1;
    s1 = s2;
    s2 = sTemp;
}

bool Magic3D::SoundOGG::isStarted()
{
    return started;
}

Magic3D::XMLElement* Magic3D::SoundOGG::save(XMLElement* root)
{
    Sound::save(root);
    if (root)
    {

    }
    return root;
}

Magic3D::XMLElement* Magic3D::SoundOGG::load(XMLElement* root)
{
    Sound::load(root);
    if (root)
    {

    }
    return root;
}
