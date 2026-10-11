//==============================================================================
//Copyright 2003-2012 Robert Pelloni.
//All Rights Reserved.
//==============================================================================



#include "../../main.h"


#define RANGE_TOWNDoctorsOfficeEntranceToOutside 4*8,29*8,8*8,30*8
#define RANGE_TOWNDoctorsOfficeEntranceToDoctorsOfficeBackHall 4*8,10*8,8*8,11*8
#define RANGE_TOWNDoctorsOfficeBackHallToDoctorsOfficeEntrance 4*8,29*8,8*8,30*8
#define RANGE_TOWNDoctorsOfficeBackHallToDoctorsOfficeBackRoom 5*8,10*8,9*8,11*8
#define RANGE_TOWNDoctorsOfficeBackRoomToDoctorsOfficeBackHall 5*8,24*8,9*8,25*8


//========================================================
//TOWNDoctorsOfficeEntrance
//========================================================
void bobsgame_TOWNDoctorsOfficeEntrance_Map_Load_Function()
{
//RANGE_TOWNDoctorsOfficeEntranceToOutside 4*8,29*8,8*8,30*8
//RANGE_TOWNDoctorsOfficeEntranceToDoctorsOfficeBackHall 4*8,10*8,8*8,11*8
}

void bobsgame_TOWNDoctorsOfficeEntrance_Map_Run_Function(int MAP_just_loaded)
{
	if(MAP_just_loaded==1)
	{

	}

//TODO: fish in fish tank
//receptionist 16,7, under 3, down
	static NPC* receptionist_npc = NULL;
	if(receptionist_npc==NULL)
	{
		NPC_create_npc_xy_feet(&receptionist_npc,GFX_ADULT_nurse,16,40,17*8,8*8-3);
		receptionist_npc->non_walkable=1;
		receptionist_npc->layer=3;
		NPC_animate_stand_dir(&receptionist_npc, DOWN);
	}
	else if(receptionist_npc!=NULL)
	{
		if(ACTION_range_xy_xy(15*8,9*8,19*8,11*8,"Make Appointment"))
		{
			TEXT_set_sprite_window(0,receptionist_npc,NULL);
			TEXT_set_sprite_window(1,PLAYER_npc,NULL);
			//CONTINUED (Nick): 3-round appointment-desk dialogue for the entrance receptionist. New completion dialogue, not Bob-authored.
			static int receptionist_talks = 0;
			if(receptionist_talks==0)
			{
				TEXT_window(
				"Welcome to Dr. Proctologist's office. Do Yuu have an appointment?<.>"
				"<1>Uh, no. I just walked in.<.>"
				"<0>*sigh*<.>"
				"Name?<.>"
				"<1>Yuu.<.>"
				"<0>Reason for visit?<.>"
				"<1>I'm just looking around.<.>"
				"<0>That's not a medical reason, Yuu.<.>"
				"<1>Isn't this a proctology place? I'm twelve.<.>"
				"<0>And I'm the receptionist. We all have our crosses to bear.<.>"
				"<1>...<.>"
				"<0>The earliest appointment is Thursday.<.>"
				"<1>This Thursday?<.>"
				"<0>*chuckle*<.>"
				"No."
				);
			}
			else if(receptionist_talks==1)
			{
				TEXT_window(
				"<0>Back again? Did Yuu lose Yuu's appointment?<.>"
				"<1>I never had one.<.>"
				"<0>Then Yuu is in the wrong line. The no-appointment line is outside.<.>"
				"<1>There's no line outside.<.>"
				"<0>Exactly. No demand.<.>"
				"Now, would Yuu like to make an appointment?<.>"
				"<1>Sure, I guess.<.>"
				"<0>Wonderful. Let me check the book... it's completely full.<.>"
				"<1>Completely?<.>"
				"<0>Every page. Eleven years of appointments. The doctor is very popular.<.>"
				"<1>Can I see the book?<.>"
				"<0>Absolutely not. The book is for staff only. It contains... appointments."
				);
			}
			else
			{
				TEXT_window(
				"<0>Yuu wants to know something about the appointment book?<.>"
				"<1>Not really.<.>"
				"<0>Too bad. Nobody has ever kept an appointment here.<.>"
				"<1>What?<.>"
				"<0>They make the appointment, I write it in the book, and then they never come.<.>"
				"Eleven years. Not one kept appointment.<.>"
				"<1>Then why do you keep making them?<.>"
				"<0>Because if I stop, they'll realize they don't need a receptionist.<.>"
				"<1>...<.>"
				"<0>*chuckle*<.>"
				"Now. Would Yuu like to make an appointment? The book is hungry."
				);
			}
			if(receptionist_talks<2)receptionist_talks++;
		}
	}


	//patients
//14,26 up
//18,26 up
//22,26 up
//26,26 up
//17,22 left
//17,18 left
//20,20 right
//20,22 right
//30,18 left
//30,22 left
	static NPC* patients_npcs[10] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	int c=0;
	for(c=0;c<10;c++)
	{
		if(patients_npcs[c]==NULL)
		{
			int x=0;
			int y=0;
			int dir=UP;
			int bgp=2;
			if(c== 0){x=14*8;y=27*8-4;dir=UP;}
			if(c== 1){x=18*8;y=27*8-4;dir=UP;}
			if(c== 2){x=22*8;y=27*8-4;dir=UP;}
			if(c== 3){x=26*8;y=27*8-4;dir=UP;}
			if(c== 4){x=17*8;y=18*8;dir=LEFT;}
			if(c== 5){x=17*8;y=22*8-4;dir=LEFT;}
			if(c== 6){x=21*8;y=20*8;dir=RIGHT;}
			if(c== 7){x=21*8;y=22*8-4;dir=RIGHT;}
			if(c== 8){x=30*8;y=18*8;dir=LEFT;}
			if(c== 9){x=30*8;y=22*8-4;dir=LEFT;}
			NPC_create_random_ADULT_xy_feet(&patients_npcs[c],x,(y)+3);
			NPC_animate_stand_dir(&patients_npcs[c], dir);
			patients_npcs[c]->non_walkable=1;
			patients_npcs[c]->layer=bgp;
		}
		else if(patients_npcs[c]!=NULL)
		{
			//if(NPC_in_range_of_npc_amt(&PLAYER_npc,patients_npcs[c],16))NPC_stare_at_npc(&patients_npcs[c],PLAYER_npc);
			//else NPC_animate_set_direction_now(&patients_npcs[c], UP);
			//if(ACTION_npc(&patients_npcs[c],"Talk To Patient"))
			//{
			//	TEXT_set_sprite_window(0,patients_npcs[c],NULL);
			//	TEXT_set_sprite_window(1,PLAYER_npc,NULL);
			//	if(c==0)TEXT_window("");
			//}
		}
	}
	if(walking_into_door(RANGE_TOWNDoctorsOfficeEntranceToOutside))
	{
		MAP_change_map(MAP_bobsgame_TOWNTown,402,300);return;
	}
	if(walking_into_door(RANGE_TOWNDoctorsOfficeEntranceToDoctorsOfficeBackHall))
	{
		MAP_change_map(MAP_bobsgame_TOWNDoctorsOfficeBackHall,6,28);return;
	}
}
void bobsgame_TOWNDoctorsOfficeEntrance_Map_Stop_Function()
{
	NPC_delete_all_npcs();
}
void bobsgame_TOWNDoctorsOfficeEntrance_Map_VBL_Function()
{

}

void bobsgame_TOWNDoctorsOfficeEntrance_Map_HBL_Function()
{

}

//========================================================
//TOWNDoctorsOfficeBackHall
//========================================================
void bobsgame_TOWNDoctorsOfficeBackHall_Map_Load_Function()
{
//RANGE_TOWNDoctorsOfficeBackHallToDoctorsOfficeEntrance 4*8,29*8,8*8,30*8
//RANGE_TOWNDoctorsOfficeBackHallToDoctorsOfficeBackRoom 5*8,10*8,9*8,11*8
}

void bobsgame_TOWNDoctorsOfficeBackHall_Map_Run_Function(int MAP_just_loaded)
{
	if(MAP_just_loaded==1)
	{

	}

//16,24 down
//23,18 down
//10,20-10,25 talk to
	static NPC* receptionist_npc = NULL;
	if(receptionist_npc==NULL)
	{
		NPC_create_npc_xy_feet(&receptionist_npc,GFX_ADULT_nurse,16,40,15*8,23*8);
		receptionist_npc->non_walkable=1;
		receptionist_npc->layer=1;
		NPC_animate_stand_dir(&receptionist_npc, DOWN);
	}
	else if(receptionist_npc!=NULL)
	{
		if(ACTION_range_xy_xy(10*8,20*8,11*8,25*8,"Talk To Nurse"))
		{
			TEXT_set_sprite_window(0,receptionist_npc,NULL);
			TEXT_set_sprite_window(1,PLAYER_npc,NULL);
			//CONTINUED (Nick): 3-round dialogue for the back-hall nurse. New completion dialogue, not Bob-authored.
			static int backnurse_talks = 0;
			if(backnurse_talks==0)
			{
				TEXT_window(
				"<0>Hold it. This hallway is for staff. Patients stay up front.<.>"
				"<1>I'm not a patient. I'm just looking around.<.>"
				"<0>That's what the last health inspector said.<.>"
				"<1>There was a health inspector?<.>"
				"<0>Once. We don't talk about it.<.>"
				"<1>...<.>"
				"<0>Yuu doesn't look sick. Yuu looks like a kid who's somewhere he's not supposed to be.<.>"
				"<1>I'm fine, really.<.>"
				"<0>That's exactly what the sick ones say. The healthy ones just leave."
				);
			}
			else if(backnurse_talks==1)
			{
				TEXT_window(
				"<0>Yuu again. Still not a patient?<.>"
				"<1>Still just looking.<.>"
				"<0>Mm-hm. Yuu knows what's through that door, right?<.>"
				"<1>The doctor's office?<.>"
				"<0>The examination room. Table, stirrups, the big light.<.>"
				"<1>What's it for?<.>"
				"<0>Proctology.<.>"
				"<1>I'm twelve.<.>"
				"<0>And yet here Yuu is, in the hallway, asking questions. Curiosity is the first symptom."
				);
			}
			else
			{
				TEXT_window(
				"<0>Yuu wants the truth about this place?<.>"
				"<1>Okay.<.>"
				"<0>The doctor hasn't come out of that room in three days.<.>"
				"<1>Is he okay?<.>"
				"<0>He's in there with a patient. Has been since Tuesday.<.>"
				"<1>Since Tuesday?!<.>"
				"<0>The patient keeps saying just one more question. The doctor can't get a word in.<.>"
				"<1>...<.>"
				"<0>*chuckle*<.>"
				"I'm only half joking. Go on, knock. See what happens."
				);
			}
			if(backnurse_talks<2)backnurse_talks++;
		}
	}
	static NPC* receptionist2_npc = NULL;
	if(receptionist2_npc==NULL)
	{
		NPC_create_npc_xy_feet(&receptionist2_npc,GFX_ADULT_nurse,16,40,22*8,17*8);
		receptionist2_npc->non_walkable=1;
		receptionist2_npc->layer=2;
		NPC_animate_stand_dir(&receptionist2_npc, DOWN);
	}
	else if(receptionist2_npc!=NULL)
	{
		//CONTINUED (Nick): this nurse had no talk action at all. Giving her one + 3-round dialogue.
		//She stands behind the sealed counter (unreachable tiles), so the talk range sits on
		//the player-accessible counter front at her height -- talk-across-the-counter, like the intake window.
		if(ACTION_range_xy_xy(9*8,15*8,11*8,19*8,"Talk To Nurse"))
		{
			TEXT_set_sprite_window(0,receptionist2_npc,NULL);
			TEXT_set_sprite_window(1,PLAYER_npc,NULL);
			//CONTINUED (Nick): 3-round dialogue for the second back-hall nurse. New completion dialogue, not Bob-authored.
			static int receptionist2_talks = 0;
			if(receptionist2_talks==0)
			{
				TEXT_window(
				"<1>Hi.<.>"
				"<0>Oh! Yuu startled me.<.>"
				"<1>Sorry.<.>"
				"<0>No, it's fine. I'm just... new. Well, new-ish. Eleven years new.<.>"
				"<1>That's not new.<.>"
				"<0>It is here. The other nurse has been here since the building was built.<.>"
				"<1>How old is the building?<.>"
				"<0>Older than the town. Don't ask."
				);
			}
			else if(receptionist2_talks==1)
			{
				TEXT_window(
				"<0>Yuu came back. Nobody comes back here twice.<.>"
				"<1>Why not?<.>"
				"<0>Because this hallway only goes to the examination room, and nobody wants to go in there twice.<.>"
				"<1>What's wrong with it?<.>"
				"<0>Have you met the doctor?<.>"
				"<1>Not yet.<.>"
				"<0>Keep it that way. He's very... thorough.<.>"
				"<1>...<.>"
				"<0>*chuckle*<.>"
				"That will be $10,000."
				);
			}
			else
			{
				TEXT_window(
				"<0>Can I tell Yuu a secret?<.>"
				"<1>Sure.<.>"
				"<0>I was a patient once. Came in for a checkup.<.>"
				"<1>And?<.>"
				"<0>That was eleven years ago. I never left.<.>"
				"<1>What?!<.>"
				"<0>The benefits are great. The hours are terrible. The doctor is... thorough.<.>"
				"<1>...<.>"
				"<0>*chuckle*<.>"
				"Don't look so scared, Yuu. Yuu can leave whenever Yuu wants.<.>"
				"Probably."
				);
			}
			if(receptionist2_talks<2)receptionist2_talks++;
		}
	}
	if(walking_into_door(RANGE_TOWNDoctorsOfficeBackHallToDoctorsOfficeEntrance))
	{
		MAP_change_map(MAP_bobsgame_TOWNDoctorsOfficeEntrance,6,11);return;
	}
	if(walking_into_door(RANGE_TOWNDoctorsOfficeBackHallToDoctorsOfficeBackRoom))
	{
		MAP_change_map(MAP_bobsgame_TOWNDoctorsOfficeBackRoom,7,23);return;
	}
}
void bobsgame_TOWNDoctorsOfficeBackHall_Map_Stop_Function()
{
	NPC_delete_all_npcs();
}
void bobsgame_TOWNDoctorsOfficeBackHall_Map_VBL_Function()
{

}

void bobsgame_TOWNDoctorsOfficeBackHall_Map_HBL_Function()
{

}

//========================================================
//TOWNDoctorsOfficeBackRoom
//========================================================
void bobsgame_TOWNDoctorsOfficeBackRoom_Map_Load_Function()
{
//RANGE_TOWNDoctorsOfficeBackRoomToDoctorsOfficeBackHall 5*8,24*8,9*8,25*8
}

void bobsgame_TOWNDoctorsOfficeBackRoom_Map_Run_Function(int MAP_just_loaded)
{
	if(MAP_just_loaded==1)
	{

	}

//20,15-24,24 sit in bed
//doctor at 18,20 face down
//22,22 lay down
	static NPC* doctor_npc = NULL;
	if(doctor_npc==NULL)
	{
		NPC_create_npc_xy_feet(&doctor_npc,GFX_ADULT_familydoctordentist,16,40,18*8,19*8);
		doctor_npc->non_walkable=1;
		//doctor_npc->layer=1;
		NPC_animate_stand_dir(&doctor_npc, DOWN);
	}
	else if(doctor_npc!=NULL)
	{
		if(ACTION_npc(&doctor_npc,"What's Up, Doc?"))
		{
			TEXT_set_sprite_window(0,doctor_npc,NULL);
			TEXT_set_sprite_window(1,PLAYER_npc,NULL);
			TEXT_window("Very funny. Could I have Yuu lay down and take his shirt off?<.>"
			"<1>Why do I need to take off my shirt?<.>"
			"<0>Don't ask foolish questions."
			);
		}
	}
	if(ACTION_range_xy_xy(20*8,16*8,24*8,24*8,"Lay Down"))
	{
		//TODO: move sprite to 22,22 down
		//use shirtless animation
		//doctor gets up, moves next to table
			TEXT_set_sprite_window(0,doctor_npc,NULL);
		TEXT_set_sprite_window(1,PLAYER_npc,NULL);
		TEXT_window("Ah, hmm. I see. I see.<.>"
		"<1>What is it, doc? Is it bad? I can take it!<.>"
		"<0>I suppose I should just tell Yuu. Are you prepared for a great shock?<.>"
		"<1>I'm scared. Don't tell me.<.>"
		"<0>Yuu is going to die.<.>"
		"<1>*sob*<.>"
		"I knew it. How much time do I have left? Can I spend one last day with my family?<.>"
		"<0>Let's see, Yuu is 12? Probably about 60 years left, on average.<.>"
		"*chuckle*<.>"
		"<1>That's horrible.<.>"
		"<0>Don't worry. Just a little doctor humor.<.>"
		"To be honest, Yuu appears to have an untreatable form of mental illness, though I'm not a specialist in that field.<.>"
		"Yuu might already be seeing symptoms of it.<.>"
		"For instance, does Yuu ever feel like people are speaking about Yuu in third person, or talking in a strange context?<.>"
		"Or perhaps Yuu feels that his actions are controlled by some unknown outside force?<.>"
		"Yuu might be speaking out uncontrollably, or talking to himself, or hearing voices.<.>"
		"These are the usual signs of psychosis. Nothing to worry about.<.>"
		"Yuu can live out the rest of his life quite comfortably in a nice soft bed.<.>"
		"See? That doesn't sound so bad now, does it?<.>"
		"<1>That's worse than before! Tell me you're joking!<.>"
		"<0>*chuckle*<.>"
		"Maybe, maybe not.<.>"
		"Now if Yuu could excuse me, this is a proctology examination center, not a pediatric clinic.<.>"
		"Unless...<.>"
		"<1>I'm leaving.<.>"
		"<0>That will be $10,000. Please see the receptionist on the way out.<.>"
		"I hope Yuu has insurance.<.>"
		"*chuckle*"
		);
	}
	if(walking_into_door(RANGE_TOWNDoctorsOfficeBackRoomToDoctorsOfficeBackHall))
	{
		MAP_change_map(MAP_bobsgame_TOWNDoctorsOfficeBackHall,7,11);return;
	}
}
void bobsgame_TOWNDoctorsOfficeBackRoom_Map_Stop_Function()
{
	NPC_delete_all_npcs();
}
void bobsgame_TOWNDoctorsOfficeBackRoom_Map_VBL_Function()
{

}

void bobsgame_TOWNDoctorsOfficeBackRoom_Map_HBL_Function()
{

}


