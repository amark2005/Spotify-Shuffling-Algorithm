#include <raylib.h>
#include <raygui.h>
#include <fstream>
#include <string>
#include <iostream>
#include<nlohmann/json.hpp>
#include <array>
#define SONGCOUNT 26
using std::array;
using std::cout;
using std::string;
using json=nlohmann::json;
int songcountloader=0;
struct Song {
  std::string song_name;
  int song_id;
  std::string artist_name;
  int artist_id;
  int duration_ms;
  int play_count;
  int skip_count;
  bool liked;
  float weight_score;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Song,song_name,song_id,artist_name,artist_id,duration_ms,play_count,skip_count,liked,weight_score);
Song songdata[SONGCOUNT];
int loadsong(){
  std::ifstream file("song.json");
  if(!file.is_open()){
    cout<<"No json file is found\n";
    return 1;
  }
  json data;
  file>>data;
  int count = data.size();
    if(count > SONGCOUNT) count = SONGCOUNT;

    for(int i = 0; i < count; i++)
    {
        songdata[i].song_name   = data[i]["song_name"].get<string>();
        songdata[i].song_id     = data[i]["song_id"].get<int>();
        songdata[i].artist_name = data[i]["artist_name"].get<string>();
        songdata[i].artist_id   = data[i]["artist_id"].get<int>();
        songdata[i].duration_ms = data[i]["duration_ms"].get<int>();
        songdata[i].play_count  = data[i]["play_count"].get<int>();
        songdata[i].skip_count  = data[i]["skip_count"].get<int>();
        songdata[i].liked       = data[i]["liked"].get<bool>();
        songdata[i].weight_score= data[i]["weight_score"].get<float>();
    }
    songcountloader=count;
return 0;
}




int main(){
  loadsong();
  cout<<"Total Number of Songs Loaded: "<<songcountloader;
  int wid=500;
  int heig=300;
  float centwid=wid/2.0f;
  float centhei=heig/2.0f;
  int musiccount=1;
  InitWindow(wid,heig,"Shuffle");

  while(!WindowShouldClose()){
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Now Playing",centwid,35,12,WHITE);
    if (GuiButton((Rectangle){centwid+50,100,100,40}, "Next Song")) musiccount++;
    if(GuiButton((Rectangle){centwid-50,100,100,40},"Previous Song")) musiccount--;
    DrawText(songdata[musiccount].song_name.c_str(),centwid,60,18,WHITE);
    
    
    EndDrawing();
  }
  CloseWindow();
}