#include <memory>

#include "SDL_mixer.h"

#include "kaacore/engine.h"
#include "kaacore/exceptions.h"
#include "kaacore/log.h"

#include "kaacore/audio.h"

namespace kaacore {

const uint32_t event_music_finished = SDL_RegisterEvents(1);
const uint32_t event_channel_finished = SDL_RegisterEvents(1);
const uint16_t default_mixing_channels_count = 32;

SoundData::SoundData(Mix_Chunk* raw_sound) : _raw_sound(raw_sound) {}

SoundData::~SoundData()
{
    if (this->_raw_sound) {
        Mix_FreeChunk(this->_raw_sound);
    }
}

Resource<SoundData>
SoundData::load(const char* path)
{
    auto raw_sound = get_engine()->audio_manager->load_raw_sound(path);
    return std::make_shared<SoundData>(raw_sound);
}

Sound::Sound() : _volume(1.) {}

Sound::Sound(Resource<SoundData> sound_data, double volume)
    : _sound_data(sound_data), _volume(volume)
{}

Sound
Sound::load(const char* path, double volume)
{
    return Sound(SoundData::load(path), volume);
}

double
Sound::volume() const
{
    return this->_volume;
}

void
Sound::volume(const double vol)
{
    this->_volume = vol;
}

Sound::operator bool() const
{
    return bool(this->_sound_data);
}

bool
Sound::operator==(const Sound& other) const
{
    return this->_sound_data == other._sound_data;
}

void
Sound::play(double volume_factor)
{
    KAACORE_ASSERT(get_engine()->audio_manager);
    get_engine()->audio_manager->play_sound(
        *this, this->_volume * volume_factor);
}

MusicData::MusicData(Mix_Music* raw_music) : _raw_music(raw_music) {}

MusicData::~MusicData()
{
    if (this->_raw_music) {
        Mix_FreeMusic(this->_raw_music);
    }
}

Resource<MusicData>
MusicData::load(const char* path)
{
    auto raw_music = get_engine()->audio_manager->load_raw_music(path);
    return std::make_shared<MusicData>(raw_music);
}

Music::Music() : _volume(1.) {}

Music::Music(Resource<MusicData> music_data, double volume)
    : _music_data(music_data), _volume(volume)
{}

Music
Music::load(const char* path, double volume)
{
    return Music(MusicData::load(path), volume);
}

Music
Music::get_current()
{
    KAACORE_ASSERT(get_engine()->audio_manager);
    return get_engine()->audio_manager->_music_state.current_music;
}

MusicState
Music::get_state()
{
    KAACORE_ASSERT(get_engine()->audio_manager);
    return get_engine()->audio_manager->music_state();
}

double
Music::volume() const
{
    return this->_volume;
}

void
Music::volume(const double vol)
{
    this->_volume = vol;
}

Music::operator bool() const
{
    return bool(this->_music_data);
}

bool
Music::operator==(const Music& other) const
{
    return this->_music_data == other._music_data;
}

bool
Music::is_playing() const
{
    KAACORE_ASSERT(get_engine()->audio_manager);
    return *this == this->get_current() and
           this->get_state() == MusicState::playing;
}

void
Music::play(double volume_factor)
{
    KAACORE_ASSERT(get_engine()->audio_manager);
    get_engine()->audio_manager->play_music(
        *this, this->_volume * volume_factor);
}

bool
Music::is_paused() const
{
    KAACORE_ASSERT(get_engine()->audio_manager);
    if (this->get_current() == *this and
        this->get_state() == MusicState::paused) {
        return true;
    }
    return false;
}

bool
Music::pause()
{
    KAACORE_ASSERT(get_engine()->audio_manager);
    if (this->get_current() == *this and
        this->get_state() == MusicState::playing) {
        get_engine()->audio_manager->_pause_music();
        return true;
    }
    return false;
}

bool
Music::resume()
{
    KAACORE_ASSERT(get_engine()->audio_manager);
    if (this->get_current() == *this and
        this->get_state() == MusicState::paused) {
        get_engine()->audio_manager->_resume_music();
        return true;
    }
    return false;
}

bool
Music::stop()
{
    KAACORE_ASSERT(get_engine()->audio_manager);
    auto state = this->get_state();
    if (this->get_current() == *this and
        (state == MusicState::paused or state == MusicState::playing)) {
        get_engine()->audio_manager->_stop_music();
        return true;
    }
    return false;
}

void
_music_finished_hook()
{
    SDL_Event event;
    event.type = event_music_finished;
    SDL_PushEvent(&event);
}

void
_channel_finished_hook(int channel)
{
    SDL_Event event;
    event.type = event_channel_finished;
    event.user.code = channel;
    SDL_PushEvent(&event);
}

AudioManager::AudioManager()
    : _master_volume(1.), _master_sound_volume(1.), _master_music_volume(1.)
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_Init(0); // no libraries, just WAV support
    auto err_code =
        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048);
    if (err_code == -1) {
        log<LogLevel::error>("Failed to open audio (%s)", Mix_GetError());
        return;
    }
    Mix_HookMusicFinished(_music_finished_hook);
    Mix_ChannelFinished(_channel_finished_hook);
    this->_channels_state.resize(MIX_CHANNELS);
    this->mixing_channels(default_mixing_channels_count);
}

AudioManager::~AudioManager()
{
    Mix_CloseAudio();
    Mix_Quit();
    Mix_HookMusicFinished(nullptr);
    Mix_ChannelFinished(nullptr);
}

Mix_Chunk*
AudioManager::load_raw_sound(const char* path)
{
    auto raw_sound = Mix_LoadWAV(path);
    if (not raw_sound) {
        log<LogLevel::error>(
            "Failed to load sound from path %s (%s)", path, Mix_GetError());
    }
    return raw_sound;
}

Mix_Music*
AudioManager::load_raw_music(const char* path)
{
    auto raw_music = Mix_LoadMUS(path);
    if (not raw_music) {
        log<LogLevel::error>(
            "Failed to load music from path %s (%s)", path, Mix_GetError());
    }
    return raw_music;
}

void
AudioManager::play_sound(const Sound& sound, const double volume_factor)
{
    KAACORE_ASSERT(bool(sound));
    if (sound._sound_data->_raw_sound) {
        auto channel = Mix_PlayChannel(-1, sound._sound_data->_raw_sound, 0);
        if (channel < 0) {
            log<LogLevel::error>("Failed to play sound (%s)", Mix_GetError());
            return;
        }
        KAACORE_ASSERT(channel < this->_channels_state.size());
        this->_channels_state[channel].current_sound = sound;
        this->_channels_state[channel].requested_volume = volume_factor;
        this->_recalc_channel_volume(channel);
    } else {
        log<LogLevel::error>("Failed to played incorrectly loaded sound");
    }
}

void
AudioManager::play_music(const Music& music, const double volume_factor)
{
    KAACORE_ASSERT(bool(music));
    if (music._music_data->_raw_music) {
        auto err_code = Mix_PlayMusic(music._music_data->_raw_music, 1);
        if (err_code == -1) {
            log<LogLevel::error>("Failed to play music (%s)", Mix_GetError());
            return;
        }
        this->_music_state.current_music = music;
        this->_music_state.requested_volume = volume_factor;
        this->_recalc_music_volume();
    } else {
        log<LogLevel::error>("Failed to played incorrectly loaded music");
    }
}

MusicState
AudioManager::music_state()
{
    if (Mix_PlayingMusic()) {
        if (Mix_PausedMusic()) {
            return MusicState::paused;
        } else {
            return MusicState::playing;
        }
    } else {
        return MusicState::stopped;
    }
}

uint16_t
AudioManager::mixing_channels() const
{
    return Mix_AllocateChannels(-1);
}

void
AudioManager::mixing_channels(const uint16_t channels)
{
    Mix_AllocateChannels(channels);
    this->_channels_state.resize(channels);
}

double
AudioManager::master_volume() const
{
    return this->_master_volume;
}

void
AudioManager::master_volume(const double vol)
{
    this->_master_volume = vol;
    this->_recalc_music_volume();
    this->_recalc_channels_volume();
}

double
AudioManager::master_sound_volume() const
{
    return this->_master_sound_volume;
}

void
AudioManager::master_sound_volume(const double vol)
{
    this->_master_sound_volume = vol;
    this->_recalc_channels_volume();
}

double
AudioManager::master_music_volume() const
{
    return this->_master_music_volume;
}

void
AudioManager::master_music_volume(const double vol)
{
    this->_master_music_volume = vol;
    this->_recalc_music_volume();
}

void
AudioManager::_pause_music()
{
    Mix_PauseMusic();
}

void
AudioManager::_resume_music()
{
    Mix_ResumeMusic();
}

void
AudioManager::_stop_music()
{
    Mix_HaltMusic();
}

void
AudioManager::_recalc_music_volume()
{
    Mix_VolumeMusic(
        this->_master_volume * this->_master_music_volume *
        this->_music_state.requested_volume * MIX_MAX_VOLUME);
}

void
AudioManager::_recalc_channels_volume()
{
    size_t channels_count = this->_channels_state.size();
    for (size_t i = 0; i < channels_count; i++) {
        if (this->_channels_state[i].current_sound) {
            this->_recalc_channel_volume(i);
        }
    }
}

void
AudioManager::_recalc_channel_volume(uint16_t channel_id)
{
    KAACORE_ASSERT(channel_id < this->_channels_state.size());
    KAACORE_ASSERT(this->_channels_state[channel_id].current_sound);
    Mix_Volume(
        channel_id, this->_master_volume * this->_master_sound_volume *
                        this->_channels_state[channel_id].requested_volume *
                        MIX_MAX_VOLUME);
}

void
AudioManager::_handle_music_finished()
{
    log<LogLevel::debug>("Music channel finished playback");
    this->_music_state.current_music = Music(); // empty Music
}

void
AudioManager::_handle_channel_finished(uint16_t channel_id)
{
    log<LogLevel::debug>("Sound channel #%u finished playback", channel_id);
    if (channel_id < this->_channels_state.size()) {
        this->_channels_state[channel_id].current_sound =
            Sound(); // empty sound
    }
}

} // namespace kaacore
