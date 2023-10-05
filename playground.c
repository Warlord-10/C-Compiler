#include <stdio.h>
#define LEN 3
typedef struct process{
    int number;
    int AT;
    int BT;
} process;


process QUEUE[LEN];

void FCFS(){
    int t=0;
    float avg_tt=0;
    float avg_wt=0;
    printf("Process\tTT\tWT\n");
    for(int i=0;i<LEN;i++){
        t+=QUEUE[i].BT;
        printf("%d\t",QUEUE[i].number);
        printf("%d\t",t-QUEUE[i].AT);
        printf("%d\n",t-QUEUE[i].AT-QUEUE[i].BT);
        avg_tt += t-QUEUE[i].AT;
        avg_wt += t-QUEUE[i].AT-QUEUE[i].BT;
        
    }
    printf("Avg TT: %f\nAvg WT: %f",avg_tt/LEN,avg_wt/LEN);
}

void SJF(){
    for(int i=0;i<LEN-1;i++){
        for(int j=0;j<LEN-i-1;j++){
            if(QUEUE[j].BT > QUEUE[j+1].BT){
                process temp = QUEUE[j];
                QUEUE[j] = QUEUE[j+1];
                QUEUE[j+1] = temp;
            }
        }
    }
    FCFS();
}


int main(){
    for(int i=0;i<LEN;i++){
        QUEUE[i].number = i;
        printf("Arrival Time(%d): ",i);
        scanf("%d",&QUEUE[i].AT);
        printf("Burst Time: ");
        scanf("%d",&QUEUE[i].BT);
    }
    SJF();
}


/*
import pyaudio
import numpy as np

# Define parameters for recording
CHUNK = 1024  # Samples per frame
FORMAT = pyaudio.paInt16  # Audio format
CHANNELS = 1  # Mono recording
RATE = 44100  # Sample rate (Hz)
RECORD_SECONDS = 5  # Duration of recording (seconds)

# Initialize PyAudio
p = pyaudio.PyAudio()

# Open microphone stream
stream = p.open(format=FORMAT,
                channels=CHANNELS,
                rate=RATE,
                input=True,
                frames_per_buffer=CHUNK)

# Open speaker stream
output_stream = p.open(format=FORMAT,
                       channels=CHANNELS,
                       rate=RATE,
                       output=True)

# Record audio from microphone
frames = []
for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
    data = stream.read(CHUNK)
    frames.append(data)

# Convert recorded data to numpy array
audio_data = np.frombuffer(b''.join(frames), dtype=np.int16)

# Negate the array to produce opposite sound waves
audio_data_opposite = -audio_data

# Convert opposite data back to bytes
audio_bytes_opposite = audio_data_opposite.astype(np.int16).tobytes()

# Output opposite sound waves through speaker
output_stream.write(audio_bytes_opposite)

# Stop streams
stream.stop_stream()
stream.close()
output_stream.stop_stream()
output_stream.close()

# Terminate PyAudio
p.terminate()

*/
