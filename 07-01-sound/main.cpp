
#include <SDL.h>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>

static void audio_callback(void* userdata, uint8_t* stream, int len);

static auto     start         = std::chrono::high_resolution_clock::now();
static auto     finish        = start;
static uint32_t default_delay = 0;
const char*     file_name     = "highlands.wav";
#pragma pack(push, 1)
struct audio_buff
{
    uint8_t* start       = nullptr;
    size_t   size        = 0;
    size_t   current_pos = 0;

    struct
    {
        size_t frequncy = 0;
        double time     = 0.0;
        bool   use_note = false;
    } note;
};
#pragma pack(pop)

std::ostream& operator<<(std::ostream& os, const SDL_AudioSpec& spec)
{
    os << "\tfreq: " << spec.freq << '\n'
       << "\tformat: " << std::hex << spec.format << '\n'
       << "\tchannels: " << std::dec << int(spec.channels) << '\n'
       << "\tsilence: " << int(spec.silence) << '\n'
       << "\tsamples: " << spec.samples << '\n'
       << "\tsize: " << spec.size << '\n'
       << "\tcallback: " << reinterpret_cast<const void*>(spec.callback) << '\n'
       << "\tuserdata: " << spec.userdata;
    return os;
}

int main(int /*argc*/, char* /*argv*/[])

{
    using namespace std;
    ///
    SDL_Init(SDL_INIT_AUDIO);
    // load WAV file

    SDL_AudioSpec wavSpec;
    Uint32        wavLength;
    Uint8*        wavBuffer;

    SDL_LoadWAV(file_name, &wavSpec, &wavBuffer, &wavLength);

    // open audio device

    SDL_AudioDeviceID deviceId =
        SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    // play audio

    int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
    SDL_PauseAudioDevice(deviceId, 0);
    // keep application running long enough to hear the sound

    SDL_Delay(3000);
    // clean up

    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();

    ///

    //    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    //    {
    //        cerr << "Error: INIT_AUDIO " << SDL_GetError();
    //    }

    //    const char* file_name = "highlands.wav";

    //    SDL_RWops* file = SDL_RWFromFile(file_name, "rb");
    //    if (file == nullptr)
    //    {
    //        cerr << "error: can't open file: " << file_name << "\n";
    //        return EXIT_FAILURE;
    //    }

    //    SDL_AudioSpec wav_spec;
    //    Uint8*        wav_buffer;
    //    Uint32        wav_length;

    //    clog << "loading sample buffer from file: " << file_name << endl;

    //    if (SDL_LoadWAV(file_name, &wav_spec, &wav_buffer, &wav_length) ==
    //    NULL)
    //    {
    //        cerr << "Could not open audio file" << SDL_GetError();
    //    }
    //    if (SDL_OpenAudio(&wav_spec, NULL) < 0)
    //    {
    //        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    //        exit(-1);
    //    }

    //    audio_buff loaded_audio_buff{ wav_buffer, wav_length, 0, { 0, 0,
    //    false } };

    //    clog << "audio buffer from file size: " << wav_length << " B ("
    //         << wav_length / double(1024 * 1024) << ") Mb" << endl;

    //    SDL_AudioSpec disired{ .freq     = 48000,
    //                           .format   = AUDIO_S16LSB,
    //                           .channels = 2, // stereod
    //                           .silence  = 0,
    //                           .samples  = 4096, // must be power of 2
    //                           .padding  = 0,
    //                           .size     = 0,
    //                           .callback = audio_callback,
    //                           .userdata = &loaded_audio_buff };

    //    clog << "prepare disired audio specs for output device:\n"
    //         << disired << endl;

    //    SDL_AudioSpec returned{};
    //    const char*   device_name       = nullptr; // device name or
    //    nullptr const int32_t is_capture_device = 0; // 0 - play device, 1
    //    - microphone

    //    SDL_AudioDeviceID audio_device_id = SDL_OpenAudioDevice(
    //        device_name, is_capture_device, &disired, &returned, 0);
    //    if (0 == audio_device_id)
    //    {
    //        cerr << "error: failed to open audio device: " <<
    //        SDL_GetError()
    //             << std::endl;
    //        return EXIT_FAILURE;
    //    }
    //    clog << "returned audio spec for output device:\n" << returned <<
    //    endl;

    //    if (disired.format != returned.format ||
    //        disired.channels != returned.channels || disired.freq !=
    //        returned.freq)
    //    {
    //        cerr << "error: disired != returned audio device settings!";
    //        return EXIT_FAILURE;
    //    }
    //    SDL_PauseAudioDevice(audio_device_id, 0); // non-zero to pause, 0
    //    to unpause

    //    clog << "unpause audio device (start audio thread)" << endl;
    //    SDL_Delay(5000);

    //    //    bool continue_loop = true;
    //    //    while (continue_loop)
    //    //    {
    //    //        clog << "1. stop and exit\n"
    //    //             << "2. print current music position\n"
    //    //             << "3. print music time (length)\n"
    //    //             << "4. print device buffer play length\n"
    //    //             << "5. experimental device buffer play length(time
    //    between "
    //    //                "callbacks)\n"
    //    //             << "6. set default delay for audio callback(current
    //    val: "
    //    //             << default_delay << " ms)\n"
    //    //             << "7. play note(current val: " <<
    //    //             loaded_audio_buff.note.frequncy
    //    //             << ")\n"
    //    //             << ">" << flush;

    //    //        int choise = 0;
    //    //        if (!(cin >> choise))
    //    //        {
    //    //            cerr << "Error:: fail simbol" << endl;
    //    //            return EXIT_FAILURE;
    //    //        }
    //    //        switch (choise)
    //    //        {
    //    //            case 1:
    //    //                continue_loop = false;
    //    //                break;
    //    //            case 2:
    //    //                clog << "current music pos: "
    //    //                     << loaded_audio_buff.current_pos /
    //    //                            double(loaded_audio_buff.size) * 100
    //    //                     << " %" << endl;
    //    //                break;
    //    //            case 3:
    //    //            {
    //    //                size_t format_size = wav_spec.format ==
    //    AUDIO_S16LSB ? 2 :
    //    //                0;

    //    //                double time_in_minutes =
    //    double(loaded_audio_buff.size) /
    //    //                                         wav_spec.channels /
    //    format_size /
    //    //                                         wav_spec.freq / 60;

    //    //                double minutes;
    //    //                double rest_minute = modf(time_in_minutes,
    //    &minutes);
    //    //                double seconds = rest_minute * 60; // 60 seconds
    //    in one
    //    //                minute clog << "music length: " <<
    //    time_in_minutes
    //    << "
    //    //                minutes or ("
    //    //                     << minutes << ":" << seconds << ")" <<
    //    endl;
    //    //                break;
    //    //            }
    //    //            case 4:
    //    //            {
    //    //                clog << "device buffer play length: "
    //    //                     << returned.samples / double(returned.freq)
    //    <<
    //    "
    //    //                     seconds"
    //    //                     << endl;
    //    //                break;
    //    //            }
    //    //            case 5:
    //    //            {
    //    //                double elapsed_seconds =
    //    // std::chrono::duration_cast<std::chrono::duration<double>>(
    //    //                        finish - start)
    //    //                        .count();
    //    //                clog << "time between last two audio_callbacks:
    //    "
    //    //                     << elapsed_seconds << " seconds" << endl;
    //    //                break;
    //    //            }
    //    //            case 6:
    //    //            {
    //    //                clog << "input delay in milliseconds:>" <<
    //    flush;
    //    //                cin >> default_delay;
    //    //                break;
    //    //            }
    //    //            case 7:
    //    //            {
    //    //                clog << "input note frequncy: " << flush;
    //    //                cin >> loaded_audio_buff.note.frequncy;
    //    //            }
    //    //            break;
    //    //            default:
    //    //                break;
    //    //        }
    //    //    }

    //    clog << "pause audio device (stop audio thread)" << endl;
    //    // stop audio device and stop thread call our callback function
    //    SDL_PauseAudioDevice(audio_device_id, 1);

    //    clog << "close audio device" << endl;

    //    SDL_CloseAudioDevice(audio_device_id);

    //    SDL_FreeWAV(loaded_audio_buff.start);

    //    SDL_Quit();

    return EXIT_SUCCESS;
}

static void audio_callback(void* userdata, uint8_t* stream, int len)
{

    static bool first_time = true;
    if (first_time)
    {
        std::clog << "start audio_callback!" << std::endl;
        first_time = false;
    }

    // simulate long calculation (just test)
    SDL_Delay(default_delay);

    // experimental check two continius callback delta time
    start  = finish;
    finish = std::chrono::high_resolution_clock::now();

    size_t stream_len = static_cast<size_t>(len);
    // silence
    std::memset(stream, 0, static_cast<size_t>(len));

    audio_buff* audio_buff_data = reinterpret_cast<audio_buff*>(userdata);
    assert(audio_buff_data != nullptr);

    if (audio_buff_data->note.frequncy != 0)
    {
        size_t num_samples = stream_len / 2 / 2;
        double dt          = 1.0 / 48000.0;

        int16_t* output = reinterpret_cast<int16_t*>(stream);

        for (size_t sample = 0; sample < num_samples; ++sample)
        {
            double omega_t = audio_buff_data->note.time * 2.0 * 3.1415926 *
                             audio_buff_data->note.frequncy;
            double curr_sample =
                std::numeric_limits<int16_t>::max() * sin(omega_t);
            int16_t curr_val = static_cast<int16_t>(curr_sample);

            *output = curr_val;
            output++;
            *output = curr_val;
            output++;

            audio_buff_data->note.time += dt;
        }
    }
    else
    {
        while (stream_len > 0)
        {
            // copy data from loaded buffer into output stream
            const uint8_t* current_sound_pos =
                audio_buff_data->start + audio_buff_data->current_pos;

            const size_t left_in_buffer =
                audio_buff_data->size - audio_buff_data->current_pos;

            if (left_in_buffer > stream_len)
            {
                SDL_MixAudioFormat(stream, current_sound_pos, AUDIO_S16LSB, len,
                                   128);
                audio_buff_data->current_pos += stream_len;
                break;
            }
            else
            {
                // first copy rest from buffer and repeat sound from
                //                begining SDL_MixAudioFormat(stream,
                //                current_sound_pos,
                //                                            AUDIO_S16LSB,
                //                                            left_in_buffer,
                //                                            128);
                // start buffer from begining
                audio_buff_data->current_pos = 0;
                stream_len -= left_in_buffer;
            }
        }
    }
}
