# Refriger-hater

[//]: # (![game icon]&#40;"Content/Meta/icon1.png&#41;)
<img src="Content/Meta/icon1.png" width="128"/>

### **Getting Started**

#### Prerequisites:
* Unreal Engine version: 5.3.2
* Rider version: 2023.3.4

#### FYI:
* Always open IDE first and then build project to open unreal
* All C++ files will have names suffixed with "RH", as per [guidelines](https://docs.unrealengine.com/4.26/en-US/ProductionPipelines/DevelopmentSetup/CodingStandard/)
* C++ files to be stored in "Private" dir and header files in "Public" dir.

#### Multiplayer Design

##### Engine Architecture FYI:
* it is important to understand the concept of the owner or owning client. Many nodes and actors need this information. For example, when certain actors or widgets are spawned, you should also define who owns it. This differentiates how things are handled on the server and all clients. Although an actor coexists (as a replicated actor) on different clients, there are actions that are processed only on owning client instance.
  (Image and text from [here](https://www.linkedin.com/pulse/unreal-engine-networking-multiplayer-games-part-1-mika-laaksonen/))

    <img src="Docs\clientserverarch1.png"  width="400"/>
* Different unreal quirks:
  * **GameInstance**: _created when the engine is started and exists until the game is shut down. Each client and server has its own unique GameInstance. Best suited for storing persistent client info through level._
  * **GameMode**: _Only one instance of GameMode and exists on the server. If any other client tries to get access it, a null pointer is returned. Used to define the game and its rules, and store any information that clients should not know or need not to know._
  * **GameState**: _Just like GameMode, there is only one GameState, and it is replicated to every client. As such it is used to keep up-to-date data about the game in general - like scores - and basically, anything that every client should have access to. It also contains a list of all connected Players (actually PlayerStates)_
  * **PlayerState**: _Each player client has its own PlayerState which is replicated to all other clients. Usually, PlayerState is used to store data about the player. It is replicated._
  * **PlayerController**: _Each client has one PlayerController. That PlayerController exists both on the server and owning client, but it is not replicated on other clients._

#### Game Architecture"


### About

#### Intro:
Refriger-hater is a team-based, top-down multiplayer game where 2 teams of refrigerators battle against each other to collect as many resources as possible. 
It's a dying world, and the last surviving animals rely on these smart fridges for food and beer! 
Teams must choose between engaging with other team or focusing on defending and collecting as many resources as possible.

#### Plugins:
1. [Enhanced Input](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine?application_version=5.2) 

#### References: