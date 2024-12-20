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
* C++ files to be stored in "Private" directory and header files in "Public" dir.
* Prepend the Client , Server , or Multicast keywords to the beginning of the functions when they are RPCs.

### **Technical Design:**

#### Multiplayer Game Engine Architecture:
* it is important to understand the concept of the owner or owning client. Many nodes and actors need this information. For example, when certain actors or widgets are spawned, you should also define who owns it. This differentiates how things are handled on the server and all clients. Although an actor coexists (as a replicated actor) on different clients, there are actions that are processed only on owning client instance.
  (Image and text from [here](https://www.linkedin.com/pulse/unreal-engine-networking-multiplayer-games-part-1-mika-laaksonen/))

    <img src="Docs\clientserverarch1.png"  width="400"/>
* Different unreal quirks:
  * **GameInstance**: _created when the engine is started and exists until the game is shut down. Each client and server has its own unique GameInstance. Best suited for storing persistent client info through level._
  * **GameMode**: _Only one instance of GameMode and exists on the server. If any other client tries to get access it, a null pointer is returned. Used to define the game and its rules, and store any information that clients should not know or need not to know._
  * **GameState**: _Just like GameMode, there is only one GameState, and it is replicated to every client. As such it is used to keep up-to-date data about the game in general - like scores - and basically, anything that every client should have access to. It also contains a list of all connected Players (actually PlayerStates)_
  * **PlayerState**: _Each player client has its own PlayerState which is replicated to all other clients. Usually, PlayerState is used to store data about the player. It is replicated._
  * **PlayerController**: _Each client has one PlayerController. That PlayerController exists both on the server and owning client, but it is not replicated on other clients._
* Network Helpers:
  * **AGameStateBase::GetServerWorldTimeSeconds():** Returns the simulated TimeSeconds on the server, will be synchronized on client and server
  * **AController::IsLocalController():** Returns whether this Controller is a local controller.
  * **AActor::GetLocalRole():** Returns how much control the local machine has over this actor.
    * **ROLE_Authority:** current running instance of the engine has authority, i.e is in charge of this actor (whether it's replicated or not).
    * **ROLE_SimulatedProxy:** This is the standard simulation path, and is generally based on extrapolating movement based on the last known velocity.
    * **ROLE_AutonomousProxy:** This is generally only used on actors that are possessed by PlayerControllers. This just means that this actor is receiving inputs from a human controller, so when we extrapolate, we have a bit more information, and can use actual human inputs to fill in the missing info
  * **RPCs:** To declare a function as an RPC, you simply need to add Server , Client , or NetMulticast keywords to the UFUNCTION declaration.
    * **UFUNCTION(Client):** RPC that will be called on the server, but executed on the client. Caveat: If the RPC is being called from server to be executed on a client, only the client who actually owns that Actor will execute the function.
    * **UFUNCTION(Server):** Called on the client, but executed on the server. If the RPC is being called from client to be executed on the server, the client must own the Actor that the RPC is being called on.
    * **UFUNCTION(NetMulticast):** Called from the server, and then executed on the server as well as all currently connected clients. Multicast RPCs can also be called from clients but will only execute locally in this case. If they are called from the server, the server will execute them locally as well as execute them on all currently connected clients.
  * **AActor::HasAuthority():** For every Actor that exists on the server, the server has authority over that Actor - including all replicated Actors, and pre-placed (not specifically replicated) Actors. As a result, when AActor::HasAuthority() function is run on a client, and the target is an Actor that was replicated to them, it will return false. For every Actor that exists only on client, only that client will have authority over that Actor - including mainly cosmetic (zero effect to gameplay) Actors.
    <img src="Docs\rpcs.png"  width="600"/>
    
    [ref1](https://docs.unrealengine.com/4.26/en-US/InteractiveExperiences/Networking/Actors/RPCs/)
    [ref2](https://docs.unrealengine.com/4.27/en-US/InteractiveExperiences/Networking/Actors/Roles/)

#### Input:
The game uses the new input system plugin by default as the previous system is now deprecated in unreal version >5.1.
* The main `Input Mapping Context` file called  [IMC_PlayerPawn](Content/Input/IMC_PlayerPawn.uasset) will have all input playing input mappings
* The `Input Config DataAsset` file called [ICD_DefaultRef](Content/Input/ICD_DefaultRef.uasset) will hold all required references of all `Input Action` files.
* Any C++ class referencing inputs must hold references to both of these files.
* Any new input action to be added to the game must add a new entry to both of the above files similar to what has been done with other Input Actions.
* More Unreal related documentation can be found [here](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine) for blueprint or other general usage.

#### Collision

#### Unreal C++ Quick Start
##### Reflection Specifiers:
* Function specifiers:
  * **BlueprintNativeEvent:** C++ base implementation, can be expanded in Blueprints, but if we added "BlueprintNativeEvent" in the interface's fn declaration, its necessary to add the "_Implementation" in any other C++ derivation.
  * **BlueprintCallable:** Can be called from bp.
  * **Reliable** - Will always arrive, eventually. Request will be re-sent unless an acknowledgment was received
  * **Unreliable** - Not guaranteed, packet can get lost and won't retry
  * **NetMulticast** - Usually use NetMulticast for cosmetic changes, and rep notify for state changes, when NetMulticast is marked Reliable, it "ignores relevancy", so not recommended
  * **Replicated** - Replicated
  * **ReplicatedUsing = ""** - Notify on rep. i.e similar to `RepNotify` in blueprints and docs
* Property specifiers:
  * **EditAnywhere** - edit in BP editor and per-instance in level.
  * **VisibleAnywhere** - 'read-only' in editor and level. (Use for Components)
  * **EditDefaultsOnly** - hide variable per-instance, edit in BP editor only
  * **VisibleDefaultsOnly** - 'read-only' access for variable, only in BP editor (uncommon)
  * **EditInstanceOnly** - allow only editing of instance (eg. when placed in level) and not archtypes (that is static class/bp)
    <br/><br/>
  * **BlueprintReadOnly** - read-only in the Blueprint scripting (does not affect 'details'-panel)
  * **BlueprintReadWrite** - read-write access in Blueprints
    <br/><br/>
  * **Category = ""** - display only for detail panels and blueprint context menu.
* Conditional Replication:
  The .cpp file will get the 'GetLifetimeReplicatedProps' function, which inturn wll have the `DOREPLIFETIME` and `DOREPLIFETIME_CONDITION` functions. The header declaration of that function is already created for us by UE when marking a variable as replicated.
  * **COND_InitialOnly:** This property will only attempt to send on the initial bunch
  * **COND_OwnerOnly:** This property will only send to the Actor's owner
  * **COND_SkipOwner:** This property send to every connection EXCEPT the owner
  * **COND_SimulatedOnly:** This property will only send to simulated Actors
  * **COND_AutonomousOnly:** This property will only send to autonomous Actors
  * **COND_SimulatedOrPhysics:** This property will send to simulated OR bRepPhysics Acto
  * **COND_InitialOrOwner:** This property will send on the initial bunch, or to the Actor's owner
  * **COND_Custom:** This property has no particular condition, but wants the ability to toggle on/off via SetCustomIsActiveOverride
* Class specifiers:
  * Blueprintable: without Blueprintable, we cant make a child class in blueprint
* ServerRPC's need two functions '_Implementation' and '_Validate' during the c++ implementation
##### Delegates:
Delegates in Unreal are an incredibly useful way to make event-driven code
[ref3](https://benui.ca/unreal/delegates-advanced)

#### Unreal RPCs:
https://docs.unrealengine.com/4.26/en-US/InteractiveExperiences/Networking/Actors/RPCs/

#### Game Architecture:


### About

#### Intro:
Refriger-hater is a team-based, top-down multiplayer game where 2 teams of refrigerators battle against each other to collect as many resources as possible. 
It's a dying world, and the last surviving animals rely on these smart fridges for food and beer! 
Teams must choose between engaging with other team or focusing on defending and collecting as many resources as possible.

#### Plugins:
1. [Enhanced Input](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine?application_version=5.2) 

#### External Links:
Resources that were used as references or learning materials during the development of the game. 
* Unreal Engine Official C++ Documentation:
  https://dev.epicgames.com/documentation/en-us/unreal-engine/networking-and-multiplayer-in-unreal-engine?application_version=5.3
* Unreal C++ Guide:
  https://www.tomlooman.com/unreal-engine-cpp-guide/
* Network Clock:
  https://medium.com/@invicticide/accurately-syncing-unreals-network-clock-87a3f9262594
* Multiplayer Network Compendium:
  https://cedric-neukirchen.net/docs/category/multiplayer-network-compendium/
* Unreal Engine Multiplayer Tips and Tricks:
  https://wizardcell.com/unreal/multiplayer-tips-and-tricks/
* Unreal Collision Filtering:
  https://www.unrealengine.com/en-US/blog/collision-filtering
* Delegates In Unreal Engine:
  https://benui.ca/unreal/delegates-advanced
* Hits and overlaps in Unreal:
  https://dev.epicgames.com/community/learning/tutorials/zw7m/hits-and-overlaps-bp-c-multiplayer

#### References:
* Game uses the "SWorldUserWidget.cpp" script for world UI spawn logic from following repository:
  https://github.com/tomlooman/ActionRoguelike/blob/master/Source/ActionRoguelike/Private/SWorldUserWidget.cpp