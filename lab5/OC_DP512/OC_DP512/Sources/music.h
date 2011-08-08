//*********MUSIC.C*********
#define PLAY 1
#define STOP 2
#define REWIND 4
#define REST 0
#define TRUE 1
#define FALSE 0

//pitch constants
#define E 1                
#define G 2                
#define D 3              
#define C 4
#define B 5
#define F_SHARP 6
#define A 7
#define F 8
#define C_SHARP 9
#define G_SHARP 10
#define G_SHARP_PRIME 11

//duration constants
#define ONE_THIRTYSECONDTH 1942
#define ONE_SIXTEENTH 3753
#define ONE_EIGHTH 7508       
#define THREE_SIXTEENTHS 11259
#define ONE_QUARTER 15015
#define THREE_EIGHTHS 22523    
#define ONE_HALF 30030
#define ONE_TWENTYFOURTH 2502

//Song size
#define SONG_SIZE 30          //SEVEN NATION ARMY SIZE IS 11

//Note Structure
const struct Note {
	unsigned short Duration;
	unsigned char frequency_pitch;
};

typedef const struct Note Envelope;

//OC3 Interrupt used for Duration time of Note
void OC_Init3(void);

//--------Music_Play------
// play the music
// inputs: none
// outputs: none
void Music_Play(unsigned char pitch);//Envelope* song_envelope[]);

//--------Music_Stop------
// stops the music
// inputs: none
// outputs: none
void Music_Stop(void);

//--------Music_Rewind------
// Plays music from the beginning
// inputs: none
// outputs: none
void Music_Rewind(void);