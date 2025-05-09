#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <string>
#include <vector>
#include <algorithm>

class AudioEngine {
public:
    static AudioEngine* Instance();
    
    void play();
    void stop();
    void next();
    void prev();
    void addTrack(const std::string& filename);
    void removeTrack(const std::string& filename);
    
    const std::vector<std::string>& getPlaylist() const;
    std::string getCurrentTrack() const;
    
private:
    AudioEngine();
    std::vector<std::string> playlist;
    int currentTrackIndex;
};

#endif // AUDIO_ENGINE_H
