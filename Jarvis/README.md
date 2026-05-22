// # ssh-copy-id -p 22 box@10.0.0.4


git clone https://github.com/ggml-org/whisper.cpp.git
cd whisper.cpp
make
ls
./download-ggml-model.sh base
sh ./models/download-ggml-model.sh base
cmake -B build
nano Makefile 
make -j base
ffmpeg -i /home/blade83/Musik/Hörbücher/Das\ Gesetz\ der\ Anziehung\ -\ William\ Walker\ Atkinson\ \(Hörbuch\).mp3 -ar 16000 -ac 1 -c:a pcm_s16le output.wav
./build/bin/whisper-cli -f output.wav 
ls


sh ./download-ggml-model.sh base


sh ./models/download-vad-model.sh silero-v5.1.2


sh ./download-ggml-model.sh tiny
sh ./download-ggml-model.sh small
sh ./download-ggml-model.sh medium


sudo apt-get install espeak




wget https://github.com/rhasspy/piper/releases/latest/download/piper_linux_x86_64.tar.gz
tar -xvf piper_linux_x86_64.tar.gz
cd piper
wget https://huggingface.co/rhasspy/piper-voices/resolve/main/de/de_DE/thorsten/medium/de_DE-thorsten-medium.onnx
wget https://huggingface.co/rhasspy/piper-voices/resolve/main/de/de_DE/thorsten/medium/de_DE-thorsten-medium.onnx.json
