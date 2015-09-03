/* Copyright (C) 2014-2015 Mathew Reiss
 *
 * Released under the MIT Licence, see LICENSE for details
 */

#include <pebble.h>

#define DATA 9000
	
#define openingTitle0 0		
	#define openingScore0_critic 1
	#define openingScore0_audience 501
#define openingTitle1 10 	
	#define openingScore1_critic 11
	#define openingScore1_audience 511
#define openingTitle2 20 	
	#define openingScore2_critic 21
	#define openingScore2_audience 521
#define openingTitle3 30 	
	#define openingScore3_critic 31
	#define openingScore3_audience 531
#define openingTitle4 40 	
	#define openingScore4_critic 41
	#define openingScore4_audience 541
#define boxTitle0 100 		
	#define boxScore0_critic 101
	#define boxScore0_audience 601
#define boxTitle1 110 		
	#define boxScore1_critic 111
	#define boxScore1_audience 611
#define boxTitle2 120 		
	#define boxScore2_critic 121
	#define boxScore2_audience 621
#define boxTitle3 130		
	#define boxScore3_critic 131
	#define boxScore3_audience 631
#define boxTitle4 140		
	#define boxScore4_critic 141
	#define boxScore4_audience 641
#define boxTitle5 150		
	#define boxScore5_critic 151
	#define boxScore5_audience 651
#define boxTitle6 160		
	#define boxScore6_critic 161
	#define boxScore6_audience 661
#define boxTitle7 170		
	#define boxScore7_critic 171
	#define boxScore7_audience 671
#define boxTitle8 180		
	#define boxScore8_critic 181
	#define boxScore8_audience 681
#define boxTitle9 190		
	#define boxScore9_critic 191
	#define boxScore9_audience 691
#define soonTitle0 200		
	#define soonScore0_critic 201
	#define soonScore0_audience 701
#define soonTitle1 210		
	#define soonScore1_critic 211
	#define soonScore1_audience 711 //Oh thank heaven
#define soonTitle2 220		
	#define soonScore2_critic 221
	#define soonScore2_audience 721
#define soonTitle3 230		
	#define soonScore3_critic 231
	#define soonScore3_audience 731
#define soonTitle4 240		
	#define soonScore4_critic 241
	#define soonScore4_audience 741

#define mpaa 300
#define runtime 301
#define synopsis 302
	
Window *splash_window, *my_window, *detail_window;
MenuLayer *movie_layer;
Layer *splash_layer, *detail_layer;
ScrollLayer *detail_scroll_layer;

GBitmap *splash_logo, *fresh_icon, *good_icon, *bad_icon, *na_icon;

bool animating = false, first_load = true;

char raw_score[8];

#define LIST 0
#define DETAIL 1
	
int current = 0;

AppTimer *app_timer;

char opening_title[5][64], box_title[10][64], soon_title[5][64];
int opening_score_critic[5], box_score_critic[10], soon_score_critic[5];
int opening_score_audience[5], box_score_audience[10], soon_score_audience[5];
char opening_score_total[5][32], box_score_total[10][32], soon_score_total[10][32];
int opening_icons[5], box_icons[10], soon_icons[5];

char mpaa_text[16], runtime_text[16], synopsis_text[512];
char current_title[64], mpaa_runtime[32];

MenuIndex currentIndex;
GSize title_size, detail_size;

void send_request(int key, char *request){
	Tuplet request_tuple = TupletCString(key, request);
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	dict_write_tuplet(iter, &request_tuple);
	dict_write_end(iter);
	app_message_outbox_send();
}

bool init_sent = false;

void app_timer_callback(void *data){
	if(!init_sent){
		init_sent = true;
		send_request(DATA, "init");
		layer_mark_dirty(splash_layer);
	}
}

void process_tuple(Tuple *t){
	int key = t->key;
	
	int rating = 0;
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Key: %i, Value: %s", key, t->value->cstring);
	
	switch(key){
		case openingTitle0: strcpy(opening_title[0], t->value->cstring); break;
		case openingTitle1: strcpy(opening_title[1], t->value->cstring); break;
		case openingTitle2: strcpy(opening_title[2], t->value->cstring); break;
		case openingTitle3: strcpy(opening_title[3], t->value->cstring); break;
		case openingTitle4: strcpy(opening_title[4], t->value->cstring); break;
		
		case boxTitle0: strcpy(box_title[0], t->value->cstring); break;
		case boxTitle1: strcpy(box_title[1], t->value->cstring); break;
		case boxTitle2: strcpy(box_title[2], t->value->cstring); break;
		case boxTitle3: strcpy(box_title[3], t->value->cstring); break;
		case boxTitle4: strcpy(box_title[4], t->value->cstring); break;
		case boxTitle5: strcpy(box_title[5], t->value->cstring); break;
		case boxTitle6: strcpy(box_title[6], t->value->cstring); break;
		case boxTitle7: strcpy(box_title[7], t->value->cstring); break;
		case boxTitle8: strcpy(box_title[8], t->value->cstring); break;
		case boxTitle9: strcpy(box_title[9], t->value->cstring); break;
		
		case soonTitle0: strcpy(soon_title[0], t->value->cstring); break;
		case soonTitle1: strcpy(soon_title[1], t->value->cstring); break;
		case soonTitle2: strcpy(soon_title[2], t->value->cstring); break;
		case soonTitle3: strcpy(soon_title[3], t->value->cstring); break;
		case soonTitle4: strcpy(soon_title[4], t->value->cstring); break;
		
		case openingScore0_audience: opening_score_audience[0] = atoi(t->value->cstring); break;
		case openingScore0_critic: 
			rating = atoi(t->value->cstring); 
			if(rating >= 75){ opening_icons[0] = 0; opening_score_critic[0] = rating; }
			else if(rating >= 60){ opening_icons[0] = 1; opening_score_critic[0] = rating; }
			else if(rating >= 0){ opening_icons[0] = 2; opening_score_critic[0] = rating; }
			else{ opening_icons[0] = -1; opening_score_critic[0] = -1; }
			break;
		
		case openingScore1_audience: opening_score_audience[1] = atoi(t->value->cstring); break;
		case openingScore1_critic: 
			rating = atoi(t->value->cstring); 
			if(rating >= 75){ opening_icons[1] = 0; opening_score_critic[1] = rating; }
			else if(rating >= 60){ opening_icons[1] = 1; opening_score_critic[1] = rating; }
			else if(rating >= 0){ opening_icons[1] = 2; opening_score_critic[1] = rating; }
			else{ opening_icons[1] = -1; opening_score_critic[1] = -1; }
			break;
		
		case openingScore2_audience: opening_score_audience[2] = atoi(t->value->cstring); break;
		case openingScore2_critic: 
			rating = atoi(t->value->cstring); 
			if(rating >= 75){ opening_icons[2] = 0; opening_score_critic[2] = rating; }
			else if(rating >= 60){ opening_icons[2] = 1; opening_score_critic[2] = rating; }
			else if(rating >= 0){ opening_icons[2] = 2; opening_score_critic[2] = rating; }
			else{ opening_icons[2] = -1; opening_score_critic[2] = -1; }
			break;
		
		case openingScore3_audience: opening_score_audience[3] = atoi(t->value->cstring); break;
		case openingScore3_critic: 
			rating = atoi(t->value->cstring); 
			if(rating >= 75){ opening_icons[3] = 0; opening_score_critic[3] = rating; }
			else if(rating >= 60){ opening_icons[3] = 1; opening_score_critic[3] = rating; }
			else if(rating >= 0){ opening_icons[3] = 2; opening_score_critic[3] = rating; }
			else{ opening_icons[3] = -1; opening_score_critic[3] = -1; }
			break;		
		
		case openingScore4_audience: opening_score_audience[4] = atoi(t->value->cstring); break;
		case openingScore4_critic: 
			rating = atoi(t->value->cstring); 
			if(rating >= 75){ opening_icons[4] = 0; opening_score_critic[4] = rating; }
			else if(rating >= 60){ opening_icons[4] = 1; opening_score_critic[4] = rating; }
			else if(rating >= 0){ opening_icons[4] = 2; opening_score_critic[4] = rating; }
			else{ opening_icons[4] = -1; opening_score_critic[4] = -1; }
			break;		
	
		case boxScore0_audience: box_score_audience[0] = atoi(t->value->cstring); break;
		case boxScore0_critic: 
			rating = atoi(t->value->cstring);
			if(rating >= 75){ box_icons[0] = 0; box_score_critic[0] = rating; }
			else if(rating >= 60){ box_icons[0] = 1; box_score_critic[0] = rating; }
			else if(rating >= 0){ box_icons[0] = 2; box_score_critic[0] = rating; }
			else{ box_icons[0] = -1; box_score_critic[0] = -1; }
			break;	
		
		case boxScore1_audience: box_score_audience[1] = atoi(t->value->cstring); break;
		case boxScore1_critic: 
			rating = atoi(t->value->cstring);
			if(rating >= 75){ box_icons[1] = 0; box_score_critic[1] = rating; }
			else if(rating >= 60){ box_icons[1] = 1; box_score_critic[1] = rating; }
			else if(rating >= 0){ box_icons[1] = 2; box_score_critic[1] = rating; }
			else{ box_icons[1] = -1; box_score_critic[1] = -1; }
			break;	
		
		case boxScore2_audience: box_score_audience[2] = atoi(t->value->cstring); break;
		case boxScore2_critic: 
			rating = atoi(t->value->cstring);
			if(rating >= 75){ box_icons[2] = 0; box_score_critic[2] = rating; }
			else if(rating >= 60){ box_icons[2] = 1; box_score_critic[2] = rating; }
			else if(rating >= 0){ box_icons[2] = 2; box_score_critic[2] = rating; }
			else{ box_icons[2] = -1; box_score_critic[2] = -1; }
			break;			
		
		case boxScore3_audience: box_score_audience[3] = atoi(t->value->cstring); break;
		case boxScore3_critic: 
			rating = atoi(t->value->cstring);
			if(rating >= 75){ box_icons[3] = 0; box_score_critic[3] = rating; }
			else if(rating >= 60){ box_icons[3] = 1; box_score_critic[3] = rating; }
			else if(rating >= 0){ box_icons[3] = 2; box_score_critic[3] = rating; }
			else{ box_icons[3] = -1; box_score_critic[3] = -1; }
			break;	
		
		case boxScore4_audience: box_score_audience[4] = atoi(t->value->cstring); break;
		case boxScore4_critic: 
			rating = atoi(t->value->cstring);
			if(rating >= 75){ box_icons[4] = 0; box_score_critic[4] = rating; }
			else if(rating >= 60){ box_icons[4] = 1; box_score_critic[4] = rating; }
			else if(rating >= 0){ box_icons[4] = 2; box_score_critic[4] = rating; }
			else{ box_icons[4] = -1; box_score_critic[4] = -1; }
			break;	
		
		case boxScore5_audience: box_score_audience[5] = atoi(t->value->cstring); break;
		case boxScore5_critic: 
			rating = atoi(t->value->cstring);
			if(rating >= 75){ box_icons[5] = 0; box_score_critic[5] = rating; }
			else if(rating >= 60){ box_icons[5] = 1; box_score_critic[5] = rating; }
			else if(rating >= 0){ box_icons[5] = 2; box_score_critic[5] = rating; }
			else{ box_icons[5] = -1; box_score_critic[5] = -1; }
			break;	
		
		case boxScore6_audience: box_score_audience[6] = atoi(t->value->cstring); break;
		case boxScore6_critic: 
			rating = atoi(t->value->cstring);
			if(rating >= 75){ box_icons[6] = 0; box_score_critic[6] = rating; }
			else if(rating >= 60){ box_icons[6] = 1; box_score_critic[6] = rating; }
			else if(rating >= 0){ box_icons[6] = 2; box_score_critic[6] = rating; }
			else{ box_icons[6] = -1; box_score_critic[6] = -1; }
			break;	
		
		case boxScore7_audience: box_score_audience[7] = atoi(t->value->cstring); break;
		case boxScore7_critic: 
			rating = atoi(t->value->cstring);
			if(rating >= 75){ box_icons[7] = 0; box_score_critic[7] = rating; }
			else if(rating >= 60){ box_icons[7] = 1; box_score_critic[7] = rating; }
			else if(rating >= 0){ box_icons[7] = 2; box_score_critic[7] = rating; }
			else{ box_icons[7] = -1; box_score_critic[7] = -1; }
			break;	
		
		case boxScore8_audience: box_score_audience[8] = atoi(t->value->cstring); break;
		case boxScore8_critic: 
			rating = atoi(t->value->cstring);
			if(rating >= 75){ box_icons[8] = 0; box_score_critic[8] = rating; }
			else if(rating >= 60){ box_icons[8] = 1; box_score_critic[8] = rating; }
			else if(rating >= 0){ box_icons[8] = 2; box_score_critic[8] = rating; }
			else{ box_icons[8] = -1; box_score_critic[8] = -1; }
			break;	
		
		case boxScore9_audience: box_score_audience[9] = atoi(t->value->cstring); break;
		case boxScore9_critic: 
			rating = atoi(t->value->cstring);
			if(rating >= 75){ box_icons[9] = 0; box_score_critic[9] = rating; }
			else if(rating >= 60){ box_icons[9] = 1; box_score_critic[9] = rating; }
			else if(rating >= 0){ box_icons[9] = 2; box_score_critic[9] = rating; }
			else{ box_icons[9] = -1; box_score_critic[9] = -1; }
			break;			
		
		case soonScore0_audience: soon_score_audience[0] = atoi(t->value->cstring); break;
		case soonScore0_critic: 
			rating = atoi(t->value->cstring); 
			if(rating >= 75){ soon_icons[0] = 0; soon_score_critic[0] = rating; }
			else if(rating >= 60){ soon_icons[0] = 1; soon_score_critic[0] = rating; }
			else if(rating >= 0){ soon_icons[0] = 2; soon_score_critic[0] = rating; }
			else{ soon_icons[0] = -1; soon_score_critic[0] = -1; }
			break;
		
		case soonScore1_audience: soon_score_audience[1] = atoi(t->value->cstring); break;
		case soonScore1_critic: 
			rating = atoi(t->value->cstring); 
			if(rating >= 75){ soon_icons[1] = 0; soon_score_critic[1] = rating; }
			else if(rating >= 60){ soon_icons[1] = 1; soon_score_critic[1] = rating; }
			else if(rating >= 0){ soon_icons[1] = 2; soon_score_critic[1] = rating; }
			else{ soon_icons[1] = -1; soon_score_critic[1] = -1; }
			break;
		
		case soonScore2_audience: soon_score_audience[2] = atoi(t->value->cstring); break;
		case soonScore2_critic: 
			rating = atoi(t->value->cstring); 
			if(rating >= 75){ soon_icons[2] = 0; soon_score_critic[2] = rating; }
			else if(rating >= 60){ soon_icons[2] = 1; soon_score_critic[2] = rating; }
			else if(rating >= 0){ soon_icons[2] = 2; soon_score_critic[2] = rating; }
			else{ soon_icons[2] = -1; soon_score_critic[2] = -1; }
			break;
		
		case soonScore3_audience: soon_score_audience[3] = atoi(t->value->cstring); break;
		case soonScore3_critic: 
			rating = atoi(t->value->cstring); 
			if(rating >= 75){ soon_icons[3] = 0; soon_score_critic[3] = rating; }
			else if(rating >= 60){ soon_icons[3] = 1; soon_score_critic[3] = rating; }
			else if(rating >= 0){ soon_icons[3] = 2; soon_score_critic[3] = rating; }
			else{ soon_icons[3] = -1; soon_score_critic[3] = -1; }
			break;		
		
		case soonScore4_audience: soon_score_audience[4] = atoi(t->value->cstring); break;
		case soonScore4_critic: 
			rating = atoi(t->value->cstring); 
			if(rating >= 75){ soon_icons[4] = 0; soon_score_critic[4] = rating; }
			else if(rating >= 60){ soon_icons[4] = 1; soon_score_critic[4] = rating; }
			else if(rating >= 0){ soon_icons[4] = 2; soon_score_critic[4] = rating; }
			else{ soon_icons[4] = -1; soon_score_critic[4] = -1; }
			break;			
		
		case mpaa: strcpy(mpaa_text, t->value->cstring); break;
		case runtime: strcpy(runtime_text, t->value->cstring); break;
		case synopsis: snprintf(synopsis_text, ARRAY_LENGTH(synopsis_text), "%s", t->value->cstring); break;
	}
}

void in_received_handler(DictionaryIterator *iter, void *context){
	Tuple *t = dict_read_first(iter);
	if(t){
		process_tuple(t);
	}
	
	while(t != NULL){
		t = dict_read_next(iter);
		if(t){
			process_tuple(t);
		}
	}
	
	for(int i = 0; i < 5; i++){
		if(opening_score_critic[i] != -1){
			snprintf(opening_score_total[i], sizeof(opening_score_total[i]), "C: %i%% | A: %i%%", opening_score_critic[i], opening_score_audience[i]);
		}
		else{
			snprintf(opening_score_total[i], sizeof(opening_score_total[i]), "C: %s%% | A: %i%%", "NA", opening_score_audience[i]);
		}
		
		if(soon_score_critic[i] != -1){
			snprintf(soon_score_total[i], sizeof(soon_score_total[i]), "C: %i%% | A: %i%%", soon_score_critic[i], soon_score_audience[i]);
		}
		else{
			snprintf(soon_score_total[i], sizeof(soon_score_total[i]), "C: %s%% | A: %i%%", "NA", soon_score_audience[i]);
		}
	}
	
	for(int i = 0; i < 10; i++){
		if(box_score_critic[i] != -1){
			snprintf(box_score_total[i], sizeof(box_score_total[i]), "C: %i%% | A: %i%%", box_score_critic[i], box_score_audience[i]);
		}
		else{
			snprintf(box_score_total[i], sizeof(box_score_total[i]), "C: %s%% | A: %i%%", "NA", box_score_audience[i]);
		}
	}
	
	snprintf(mpaa_runtime, sizeof(mpaa_runtime), "%s, %s", mpaa_text, runtime_text);

	menu_layer_reload_data(movie_layer);
	layer_mark_dirty(detail_layer);
	
	if(first_load){
		window_stack_push(my_window, true);
		first_load = false;
	}
	
	if(current == DETAIL){
		detail_size = GSize(144,4);
		detail_size.h += graphics_text_layout_get_content_size(current_title, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(4,0,136, 1000), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft).h;
		detail_size.h += 22;
		detail_size.h += graphics_text_layout_get_content_size(synopsis_text, fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(4, title_size.h+2+20, 136, 288), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft).h;
		
		scroll_layer_set_content_size(detail_scroll_layer, detail_size);
	}
}

void on_animation_stopped(Animation *anim, bool finished, void *context){
	property_animation_destroy((PropertyAnimation*) anim);
	
	if(current == LIST){
		strcpy(mpaa_text, "");
		strcpy(runtime_text, "");
		strcpy(synopsis_text, "");
		strcpy(mpaa_runtime, "");
	}
	
	animating = false;
}

void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay){
	animating = true;
	
	PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
	animation_set_duration((Animation*) anim, duration);
	animation_set_delay((Animation*) anim, delay);
	animation_set_curve((Animation*) anim, AnimationCurveEaseOut);
	
	AnimationHandlers handlers = {
		.stopped = (AnimationStoppedHandler) on_animation_stopped
	};
	
	animation_set_handlers((Animation*) anim, handlers, NULL);
	animation_schedule((Animation*) anim);
}

static void draw_detail(Layer *layer, GContext* ctx){
	graphics_context_set_text_color(ctx, GColorBlack);
	
	graphics_draw_text(ctx, current_title, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(4,0,136, title_size.h), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
	graphics_draw_text(ctx, mpaa_runtime, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(4, 2+title_size.h, 136, 16), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
	
	graphics_context_set_fill_color(ctx, GColorBlack);
	
	graphics_fill_rect(ctx, GRect(4, title_size.h+2+17, 136, 2), 0, GCornersAll);
	
	graphics_draw_text(ctx, synopsis_text, fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(4, title_size.h+2+20, 136, 288), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

static void draw_splash(Layer *layer, GContext* ctx){
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_context_set_text_color(ctx, GColorWhite);
	
	graphics_fill_rect(ctx, GRect(0,0,144,168), 0, GCornerNone);
	
	if(window_get_fullscreen(splash_window)){
		graphics_draw_bitmap_in_rect(ctx, splash_logo, GRect(0,72-54,144,108));
	}
	else{
		graphics_draw_bitmap_in_rect(ctx, splash_logo, GRect(0, 64-54, 144,108));
	}
	
	if(init_sent) graphics_draw_text(ctx, "Loading...", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(0,126, 144,32), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static uint16_t get_num_sections(MenuLayer *menu_layer, void *data){
	return 3;
}

static uint16_t get_num_rows(MenuLayer *menu_layer, uint16_t section_index, void *data){
	if(section_index == 0 || section_index == 2) return 5;
	else return 10;
}

void draw_header(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data){
	switch(section_index){
		case 0: menu_cell_basic_header_draw(ctx, cell_layer, "Opening This Week"); break;
		case 1: menu_cell_basic_header_draw(ctx, cell_layer, "Top Box Office"); break;
		case 2: menu_cell_basic_header_draw(ctx, cell_layer, "Coming Soon"); break;
	}
}

void draw_row(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data){
	switch(cell_index->section){
		case 0: 
		switch(opening_icons[cell_index->row]){
			case -1: menu_cell_basic_draw(ctx, cell_layer, opening_title[cell_index->row], opening_score_total[cell_index->row], na_icon); break; 
			case 0: menu_cell_basic_draw(ctx, cell_layer, opening_title[cell_index->row], opening_score_total[cell_index->row], fresh_icon); break;
			case 1: menu_cell_basic_draw(ctx, cell_layer, opening_title[cell_index->row], opening_score_total[cell_index->row], good_icon); break;
			case 2: menu_cell_basic_draw(ctx, cell_layer, opening_title[cell_index->row], opening_score_total[cell_index->row], bad_icon); break;
		}
		break;
		case 1: 
		switch(box_icons[cell_index->row]){
			case -1: menu_cell_basic_draw(ctx, cell_layer, box_title[cell_index->row], box_score_total[cell_index->row], na_icon); break; 
			case 0: menu_cell_basic_draw(ctx, cell_layer, box_title[cell_index->row], box_score_total[cell_index->row], fresh_icon); break;
			case 1: menu_cell_basic_draw(ctx, cell_layer, box_title[cell_index->row], box_score_total[cell_index->row], good_icon); break;
			case 2: menu_cell_basic_draw(ctx, cell_layer, box_title[cell_index->row], box_score_total[cell_index->row], bad_icon); break;
		}
		break;
		case 2: 
		switch(soon_icons[cell_index->row]){
			case -1: menu_cell_basic_draw(ctx, cell_layer, soon_title[cell_index->row], soon_score_total[cell_index->row], na_icon); break; 
			case 0: menu_cell_basic_draw(ctx, cell_layer, soon_title[cell_index->row], soon_score_total[cell_index->row], fresh_icon); break;
			case 1: menu_cell_basic_draw(ctx, cell_layer, soon_title[cell_index->row], soon_score_total[cell_index->row], good_icon); break;
			case 2: menu_cell_basic_draw(ctx, cell_layer, soon_title[cell_index->row], soon_score_total[cell_index->row], bad_icon); break;
		}
		break;
	}		  
}
static int16_t get_header_height(MenuLayer *menu_layer, uint16_t section_index, void *data){
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void handle_back_click(ClickRecognizerRef recognizer, void *context){
	if(animating) return;
	
	if(current == DETAIL){
		window_stack_pop(detail_window);
		
		current = LIST;
	}
	else{
		window_stack_pop_all(false);
	}
}

static void handle_down_click(ClickRecognizerRef recognizer, void *context){
	if(current == LIST)
		menu_layer_set_selected_next(movie_layer, false, MenuRowAlignCenter, true);
}

static void handle_up_click(ClickRecognizerRef recognizer, void *context){
	if(current == LIST)
		menu_layer_set_selected_next(movie_layer, true, MenuRowAlignCenter, true);
}

static void handle_select_click(ClickRecognizerRef recognizer, void *context){
	if(animating) return;
	
	if(current == LIST){
		window_stack_push(detail_window, true);
		
		strcpy(mpaa_runtime, "");
		
		currentIndex = menu_layer_get_selected_index(movie_layer);
		char request[8];
		snprintf(request, sizeof(request), "%i:%i#", (int)currentIndex.section, (int)currentIndex.row);
		send_request(DATA, request);
		
		switch(currentIndex.section){
			case 0: strcpy(current_title, opening_title[currentIndex.row]); break;
			case 1: strcpy(current_title, box_title[currentIndex.row]); break;
			case 2: strcpy(current_title, soon_title[currentIndex.row]); break;
		}
		
		title_size = graphics_text_layout_get_content_size(current_title, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GRect(4,4,136,112), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
		
		current = DETAIL;
	}
}

static void handle_up_hold(ClickRecognizerRef recognizer, void *context){
	if(current == LIST){
		MenuIndex current = menu_layer_get_selected_index(movie_layer);	
		
		if(current.section == 0 || (current.section == 1 && current.row == 0)){
			MenuIndex newIndex = MenuIndex(0,0);
			menu_layer_set_selected_index(movie_layer, newIndex, MenuRowAlignCenter, true);
		}
		else if(current.section == 1 || (current.section == 2 && current.row == 0)){
			MenuIndex newIndex = MenuIndex(1,0);
			menu_layer_set_selected_index(movie_layer, newIndex, MenuRowAlignCenter, true);			
		}
		else if(current.section == 2){
			MenuIndex newIndex = MenuIndex(2, 0);
			menu_layer_set_selected_index(movie_layer, newIndex, MenuRowAlignCenter, true);
		}
	}
}

static void handle_down_hold(ClickRecognizerRef recognizer, void *context){
	if(current == LIST){
		MenuIndex current = menu_layer_get_selected_index(movie_layer);
		
		if(current.section == 0){
			MenuIndex newIndex = MenuIndex(1,0);
			menu_layer_set_selected_index(movie_layer, newIndex, MenuRowAlignCenter, true);
		}
		else if(current.section == 1){
			MenuIndex newIndex = MenuIndex(2, 0);
			menu_layer_set_selected_index(movie_layer, newIndex, MenuRowAlignCenter, true);
		}
		else if(current.section == 2){
			MenuIndex newIndex = MenuIndex(2,4);
			menu_layer_set_selected_index(movie_layer, newIndex, MenuRowAlignCenter, true);
		}
	}
}

static void click_config(void *context){
	window_long_click_subscribe(BUTTON_ID_UP, 250, handle_up_hold, NULL);
	window_long_click_subscribe(BUTTON_ID_DOWN, 250, handle_down_hold, NULL);
	
	window_single_click_subscribe(BUTTON_ID_UP, handle_up_click);
	window_single_click_subscribe(BUTTON_ID_DOWN, handle_down_click);
	window_single_click_subscribe(BUTTON_ID_SELECT, handle_select_click);
	window_single_click_subscribe(BUTTON_ID_BACK, handle_back_click);
}

static void window_unload(Window *window){
	strcpy(mpaa_text, "");
	strcpy(runtime_text, "");
	strcpy(synopsis_text, "");
	strcpy(mpaa_runtime, "");	
	
	scroll_layer_set_content_offset(detail_scroll_layer, GPointZero, false);
	scroll_layer_set_content_size(detail_scroll_layer, GSize(144, 152));
	
	current = LIST;
}

void handle_init(void){
	app_message_register_inbox_received(in_received_handler);
	app_message_open(1024, 64);	
	
	my_window = window_create();
	
	window_set_status_bar_icon(my_window, gbitmap_create_with_resource(RESOURCE_ID_STATUS));
	
	splash_window = window_create();
	window_set_fullscreen(splash_window, true);
	window_set_click_config_provider(my_window, click_config);
	
	detail_window = window_create();
	window_set_window_handlers(detail_window, (WindowHandlers){
		.unload = window_unload
	});
	
	splash_logo = gbitmap_create_with_resource(RESOURCE_ID_SPLASH);
	fresh_icon = gbitmap_create_with_resource(RESOURCE_ID_ICON);
	good_icon = gbitmap_create_with_resource(RESOURCE_ID_GOOD);
	bad_icon = gbitmap_create_with_resource(RESOURCE_ID_BAD);
	na_icon = gbitmap_create_with_resource(RESOURCE_ID_NA);
	
	splash_layer = layer_create(GRect(0,0,144,168));
	layer_set_update_proc(splash_layer, draw_splash);

	movie_layer = menu_layer_create(GRect(0,0,144,152));
	menu_layer_set_callbacks(movie_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = get_num_sections,
		.get_num_rows = get_num_rows,
		.get_header_height = get_header_height,
		.draw_header = draw_header,
		.draw_row = draw_row
	});
	
	detail_layer = layer_create(GRect(0,0,144,1000));
	layer_set_update_proc(detail_layer, draw_detail);
	
	detail_scroll_layer = scroll_layer_create(GRect(0,0,144,152));
	
	detail_size = GSize(144,4);
	
	scroll_layer_add_child(detail_scroll_layer, detail_layer);
	
	scroll_layer_set_content_size(detail_scroll_layer, detail_size);
	
	scroll_layer_set_click_config_onto_window(detail_scroll_layer, detail_window);
	
	layer_add_child(window_get_root_layer(detail_window), scroll_layer_get_layer(detail_scroll_layer));
	
	layer_add_child(window_get_root_layer(splash_window), splash_layer);
	layer_add_child(window_get_root_layer(my_window), menu_layer_get_layer(movie_layer));
	
	window_stack_push(splash_window, true);
	
	app_timer = app_timer_register(3000, app_timer_callback, NULL);
}

void handle_deinit(void){
	gbitmap_destroy(splash_logo);
	gbitmap_destroy(fresh_icon);
	gbitmap_destroy(good_icon);
	gbitmap_destroy(bad_icon);
	gbitmap_destroy(na_icon);

	layer_destroy(splash_layer);
	layer_destroy(detail_layer);
	menu_layer_destroy(movie_layer);
	scroll_layer_destroy(detail_scroll_layer);
	
	window_destroy(splash_window);
	window_destroy(my_window);
	window_destroy(detail_window);
	
	app_message_deregister_callbacks();
}

int main(void){
	handle_init();
	app_event_loop();
	handle_deinit();
}