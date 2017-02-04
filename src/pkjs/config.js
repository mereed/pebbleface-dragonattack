module.exports = [
  {
    "type": "heading",
    "defaultValue": "Configuration"
  },
  {
    "type": "section",
    "items": [	
      {
        "type": "toggle",
        "messageKey": "anim",
        "label": "Disable or Enable animation",
        "defaultValue": true
      },
		]
		},
	   	{
  "type": "radiogroup",
  "messageKey": "flap",
  "label": "How many times should the dragon breathe fire each minute..",
  "options": [
    { 
      "label": "(1) Once", 
      "value": 1
    },
	{ 
      "label": "(2) Twice", 
      "value": 2
    },
	{ 
      "label": "3 times", 
      "value": 3
    },
	{ 
      "label": "4 times", 
      "value": 4
    },
	{ 
      "label": "5 times", 
      "value": 5
    }
   ]
  },
	    {
        "type": "text",
		"defaultValue": "<h6><em>Note: More fire means more battery usage!</em></h6>",
  },
  {
        "type": "text",
        "defaultValue": "<h6>This watchface will continue to be free.  If you find it useful, please consider making a <a href='https://www.paypal.me/markchopsreed'>small donation here</a>. Thankyou.</h6>",
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];