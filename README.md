# Demute Unreal Audio Tools

The Demute Unreal Audio Tools is an open-source plugin for Unreal containing different tools that we developed for Game Audio implementation in Unreal 4 and 5 throughout our projects. 
It currently contains two completed modules :
- Audio Animation Tools -> Automatically place tags for footsteps and foley on animation files
- Static Mesh Emitters -> Automatically place point sources in levels

If you want to know more about what we do, visit our [website](https://www.demute.studio/)
You can also contact us directly on [Discord](https://discord.gg/srNFquSUtu) with any questions you might have.

## Installation

Download the zip file from the Releases page and unzip inside the Plugins folder of your Unreal Project.
The final hierarchy should look like this :
YourAmazingProject/Plugins/DM_AudioTools/DM_AudioTools.uplugin

## Usage

For all tools, make sure you have checked "Show Plugin Content" in the Content Browser under "View Options".

### Audio Animation Tools

All of the content can be found under "DM_AudioToolsContent/AudioAnimationTools/". To quick-start, right click the AudioAnimationToolsWidget_BP and click on "Run Editor Utility Widget". This will open the AudioAnimationTools Widget. There, you can add or append animations files in a list, set up analysis and audio parameters and then generate footstep and foley event tags on your animation files.

If you need to modify any of the behavior (if you use an audio middleware for example), most of the audio logic happens in blueprints in the folder you can easily modify to suit your needs.

### Static Mesh Emitters

All of the content can be found under "DM_AudioToolsContent/StaticMeshEmitters/". To quick-start, right click the StaticMeshAudioEmittersWidget_BP and click on "Run Editor Utility Widget". This will open the StaticMeshAudioEmitters Widget. There you can add Audio Definitions to a list and generate audio emitters based on that list and the Static Meshes present in the scene. These Audio Definitions are Data Assets that can be created in the content browser of type StaticMeshAudioDefinitions_BP. In the widget, you can also delete emitters and reposition them if the object they were generated from moved.


## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change. Do not hesitate to also open issues for any bugs you encounter or features you might want to see added but can't add yourself.

## License

[GNU General Public License v3.0](https://choosealicense.com/licenses/agpl-3.0/)
