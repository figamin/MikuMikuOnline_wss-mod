
/***MetaData***

{
	"name": "Chat",
	"icon": "icon.png",
	"api_version": 1
}

***MetaData***/

// Speech baloon expiration time (ms)
var BALLOON_EXPIRATION = 40000

var list;

// Recieved chat message
var even_line = false;
Network.onReceive = function (info, msg) {

	if (msg.type && msg.type != "chat") {
		return;
	}

	var trip = ""
	if (info.player.trip()) {
		trip = " !" + info.player.trip()
	}
	
    if (msg.body && !msg.private) {
        list.addItem(
		new UI.Label({
		    docking: UI.DOCKING_TOP | UI.DOCKING_LEFT | UI.DOCKING_RIGHT,
		    text: info.player.name() + trip + ": " + msg.body,
		    bgcolor: "#FFFFFF00",
            color: "#FFFFFFFF",
            shadow_color: "#000000",
            shadow_alpha: 255
		})
	    );
    }
    
    if (msg.private) {
        list.addItem(
			new UI.Label({
		   		docking: UI.DOCKING_TOP | UI.DOCKING_LEFT | UI.DOCKING_RIGHT,
		   		text: "(PM) " + info.player.name() + trip + ": " + msg.body,
		    	bgcolor: "#FFFFFF00",
                color: "#87ceebFF",
                shadow_color: "#000000",
                shadow_alpha: 255
			})
		)
    }
    
    if (msg.system) {
        list.addItem(
		new UI.Label({
		    docking: UI.DOCKING_TOP | UI.DOCKING_LEFT | UI.DOCKING_RIGHT,
		    text: "[SERVER] " + msg.system,
		    bgcolor: "#FFFFFF00",
            color: "#FFFACDFF",
            shadow_color: "#000000",
            shadow_alpha: 255
		})
	    );
    }
    list.scroll_y = 999999;

    if (info.player && msg.body && !msg.private) {

        // Display speech baloon
        info.player.setBalloonContent(
			new UI.Label({
			    width: Screen.width() / 6,
			    text: msg.body
			})
		);

        // Speech baloon expiration timer
        clearTimeout(info.player.baloon_timer)
        info.player.baloon_timer = setTimeout(function () {
            info.player.setBalloonContent(null)
        }, BALLOON_EXPIRATION);
    }

}

Player.onLogin = function(player) {
	list.addItem(
		new UI.Label({
			docking: UI.DOCKING_TOP | UI.DOCKING_LEFT | UI.DOCKING_RIGHT,
			text: player.name() + " joined!",
            bgcolor: "#FFFFFF00",
			color: "#FFCC66FF",
            shadow_color: "#000000",
            shadow_alpha: 255,
			width: 0
		})
	);
	list.scroll_y = 999999;
}

Player.onLogout = function(player) {
	list.addItem(
		new UI.Label({
			docking: UI.DOCKING_TOP | UI.DOCKING_LEFT | UI.DOCKING_RIGHT,
			text: player.name() + "left.",
            bgcolor: "#FFFFFF00",
			color: "#CCFF66FF",
            shadow_color: "#000000",
            shadow_alpha: 255,
			width: 0
		})
	);
	list.scroll_y = 999999;
}

// Send chat message
InputBox.onEnter = function (text) {

    // For rolling dice
    var dice_parsed_text = text.match(/^\/(\d+)[Dd](\d+)/)
    if (dice_parsed_text) {
        var time = dice_parsed_text[1]
        var size = dice_parsed_text[2]
        var msg = " [Dice/" + time + "D" + size + "]\n "
        for (var i = 0; i < time; i++) {
            msg += Number.random(1, size) + ", "
        }

        var msgObject = { body: msg };
        Network.sendAll(msgObject);
        return;
    }

    // Parse the command
    //.match(/^\/(\w{1,8})\s?(\S*)/)
    var parsed_text = text.split(" ");
    var command = parsed_text[0].match(/^\/(\w*)/);
    if (command) {

        var args = parsed_text[1]

        switch (command[1]) {

            // Change nickname            
            case "nick":
                Account.updateName(args.trim());
                break;
                
            case "trip":
            	passwd = args ? args.trim() : ""
                Account.updateTrip(passwd);
                break;

            // Change model         
            case "model":
                Account.updateModelName("char:" + args.trim());
                break;

            // Reset position         
            case "escape":
                Player.escape();
                break;

            // Reload    
            case "reload":
                Model.rebuild();
                Music.rebuild();
                break;

            // System      
            case "system":
                var msgObject = { type: "system", system: args.trim() };
                Network.sendAll(msgObject);
                break;

            // Private   
            case "private":
                var msgObject = {
                	type: "chat", 
                    private: [
                    			Player.myself().id(),
                    			Player.getFromName([args.trim()]).id()
                    		 ],
                    body: parsed_text[2].trim()
                };
                Network.sendAll(msgObject);
                break;

            case "dance":
                Music.playME(parsed_text[2].trim());
                Player.playMotion(args.trim());

            case "bgm":
                Music.play(args.trim(), true);
        }

    } else {
        // Not a command, send as chat message
        var msgObject = { type: "chat", body: text };
        Network.sendAll(msgObject);
    }

};

list = new UI.List({
	docking: UI.DOCKING_TOP | UI.DOCKING_LEFT | UI.DOCKING_RIGHT | UI.DOCKING_BOTTOM
});

Card.board.width = 380;
Card.board.use_image_bg = false;
Card.board.addChild(list);