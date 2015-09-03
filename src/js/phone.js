var OPENING = 0;
var BOX_OFFICE = 1;
var COMING_SOON = 2;

var openingData;
var boxOfficeData;
var comingSoonData;

// when building a version of this, replace this value with a valid API key
// or calls to the Rotten Tomatoes API will fail.
/  API keys can be requested at http://developer.rottentomatoes.com/
var API_KEY="INVALID";

function HTTP_GET(call_type){
	try{
		var req = new XMLHttpRequest();
		switch(call_type){
			case OPENING: req.open("GET", "http://api.rottentomatoes.com/api/public/v1.0/lists/movies/opening.json?limit=5&country=us&apikey=" + API_KEY, false); break;
			case BOX_OFFICE: req.open("GET", "http://api.rottentomatoes.com/api/public/v1.0/lists/movies/box_office.json?limit=10&country=us&apikey=" + API_KEY, false); break;
			case COMING_SOON: req.open("GET", "http://api.rottentomatoes.com/api/public/v1.0/lists/movies/upcoming.json?limit=5&country=us&apikey=" + API_KEY, false); break;
		}
		req.send();
		return req.response;
	}
	catch(err){
		console.error(err);
	}
}

var openingMovies = [], boxOfficeMovies = [], comingSoonMovies = [];
var openingScores_critic = [], boxOfficeScores_critic = [], comingSoonScores_critic = [];
var openingScores_audience = [], boxOfficeScores_audience = [], comingSoonScores_audience = [];

function parseJSON(call_type, json_data){
	
	var critic_score;
	var audience_score;
	
	switch(call_type){
		case OPENING:
			for(var i = 0; i < json_data.movies.length; i++){
				openingMovies[i] = json_data.movies[i].title;
				critic_score = json_data.movies[i].ratings.critics_score;
				audience_score = json_data.movies[i].ratings.audience_score;
				openingScores_critic[i] = critic_score + "";
				openingScores_audience[i] = audience_score + "";
			}
		break;
			
		case BOX_OFFICE:
			for(var j = 0; j < json_data.movies.length; j++){
				boxOfficeMovies[j] = json_data.movies[j].title;			
				critic_score = json_data.movies[j].ratings.critics_score;
				audience_score = json_data.movies[j].ratings.audience_score;
				boxOfficeScores_critic[j] = critic_score + "";
				boxOfficeScores_audience[j] = audience_score + "";			
			}
		break;
			
		case COMING_SOON:
			for(var k = 0; k < json_data.movies.length; k++){
				comingSoonMovies[k] = json_data.movies[k].title;
				critic_score = json_data.movies[k].ratings.critics_score;
				audience_score = json_data.movies[k].ratings.audience_score;
				comingSoonScores_critic[k] = critic_score + "";
				comingSoonScores_audience[k] = audience_score + "";		
			}
		break;
	}
}

var mpaa, mpaa_string, runtime, runtime_string, synopsis;

function returnMovieDetail(section, row){
	if(section === OPENING+""){
		mpaa = openingData.movies[row].mpaa_rating;
		runtime = openingData.movies[row].runtime;
		runtime_string = Math.floor(runtime/60) + "h " + (runtime%60) + "m";
		synopsis = openingData.movies[row].synopsis.substring(0,511);
	}
	else if(section === BOX_OFFICE+""){
		mpaa = boxOfficeData.movies[row].mpaa_rating;
		runtime = boxOfficeData.movies[row].runtime;
		runtime_string = Math.floor(runtime/60) + "h " + (runtime%60) + "m";
		synopsis = boxOfficeData.movies[row].synopsis.substring(0,511);
	}
	else if(section === COMING_SOON+""){
		mpaa = comingSoonData.movies[row].mpaa_rating;
		runtime = comingSoonData.movies[row].runtime;
		runtime_string = Math.floor(runtime/60) + "h " + (runtime%60) + "m";
		synopsis = comingSoonData.movies[row].synopsis.substring(0,511);
	}
	mpaa_string = "Rating: " + mpaa;
	
	var dict3 = {"mpaa" : mpaa_string, "runtime" : runtime_string, "synopsis" : synopsis};
	Pebble.sendAppMessage(dict3);
	
}

function init(e){
	var raw = JSON.stringify(e.payload);
	var obj = JSON.parse(raw);
	var request = obj.DATA;
	
	if(request === "init"){
		openingData = JSON.parse(HTTP_GET(OPENING));
		parseJSON(OPENING, openingData);
		
		boxOfficeData = JSON.parse(HTTP_GET(BOX_OFFICE));
		parseJSON(BOX_OFFICE, boxOfficeData);
		
		comingSoonData = JSON.parse(HTTP_GET(COMING_SOON));
		parseJSON(COMING_SOON, comingSoonData);
	
		var dict = {"openingTitle0" : openingMovies[0], "openingTitle1" : openingMovies[1], "openingTitle2" : openingMovies[2], "openingTitle3" : openingMovies[3], "openingTitle4" : openingMovies[4],
				"boxTitle0" : boxOfficeMovies[0], "boxTitle1" : boxOfficeMovies[1], "boxTitle2" : boxOfficeMovies[2], "boxTitle3" : boxOfficeMovies[3], "boxTitle4" : boxOfficeMovies[4], "boxTitle5" : boxOfficeMovies[5], "boxTitle6" : boxOfficeMovies[6], "boxTitle7" : boxOfficeMovies[7], "boxTitle8" : boxOfficeMovies[8], "boxTitle9" : boxOfficeMovies[9],
				"soonTitle0" : comingSoonMovies[0], "soonTitle1" : comingSoonMovies[1], "soonTitle2" : comingSoonMovies[2], "soonTitle3" : comingSoonMovies[3], "soonTitle4" : comingSoonMovies[4],
				"openingScore0_critic" : openingScores_critic[0]+"", "openingScore1_critic" : openingScores_critic[1]+"", "openingScore2_critic" : openingScores_critic[2]+"", "openingScore3_critic" : openingScores_critic[3]+"", "openingScore4_critic" : openingScores_critic[4]+"",
				"boxScore0_critic" : boxOfficeScores_critic[0]+"", "boxScore1_critic" : boxOfficeScores_critic[1]+"", "boxScore2_critic" : boxOfficeScores_critic[2]+"", "boxScore3_critic" : boxOfficeScores_critic[3]+"", "boxScore4_critic" : boxOfficeScores_critic[4]+"", "boxScore5_critic" : boxOfficeScores_critic[5]+"", "boxScore6_critic" : boxOfficeScores_critic[6]+"", "boxScore7_critic" : boxOfficeScores_critic[7]+"", "boxScore8_critic" : boxOfficeScores_critic[8]+"", "boxScore9_critic" : boxOfficeScores_critic[9]+"",
				"soonScore0_critic" : comingSoonScores_critic[0]+"", "soonScore1_critic" : comingSoonScores_critic[1]+"", "soonScore2_critic" : comingSoonScores_critic[2]+"", "soonScore3_critic" : comingSoonScores_critic[3]+"", "soonScore4_critic" : comingSoonScores_critic[4]+"",
				"openingScore0_audience" : openingScores_audience[0]+"", "openingScore1_audience" : openingScores_audience[1]+"", "openingScore2_audience" : openingScores_audience[2]+"", "openingScore3_audience" : openingScores_audience[3]+"", "openingScore4_audience" : openingScores_audience[4]+"",
				"boxScore0_audience" : boxOfficeScores_audience[0]+"", "boxScore1_audience" : boxOfficeScores_audience[1]+"", "boxScore2_audience" : boxOfficeScores_audience[2]+"", "boxScore3_audience" : boxOfficeScores_audience[3]+"", "boxScore4_audience" : boxOfficeScores_audience[4]+"", "boxScore5_audience" : boxOfficeScores_audience[5]+"", "boxScore6_audience" : boxOfficeScores_audience[6]+"", "boxScore7_audience" : boxOfficeScores_audience[7]+"", "boxScore8_audience" : boxOfficeScores_audience[8]+"", "boxScore9_audience" : boxOfficeScores_audience[9]+"",
				"soonScore0_audience" : comingSoonScores_audience[0]+"", "soonScore1_audience" : comingSoonScores_audience[1]+"", "soonScore2_audience" : comingSoonScores_audience[2]+"", "soonScore3_audience" : comingSoonScores_audience[3]+"", "soonScore4_audience" : comingSoonScores_audience[4]+""};
		
		Pebble.sendAppMessage(dict);
	}
	else{
		var section = request.substring(0, request.indexOf(":"));
		var row = request.substring(request.indexOf(":")+1, request.indexOf("#"));
		
		returnMovieDetail(section, row);
	}
}

Pebble.addEventListener("appmessage", function(e){init(e);});