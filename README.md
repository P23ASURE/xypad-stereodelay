# XY Pad Audio Plugin

The XY Pad Audio plugin is an advanced audio tool designed to add an intriguing stereo dimension to the audio signal by dynamically manipulating delay times. Developed using the JUCE library, a renowned framework for audio software creation, this plugin stands out for its ability to control delay separately for the left and right channels, enabling users to craft custom stereo effects in real time.

## How the Plugin Works

The plugin operates on two main audio channels: left and right. It features an interactive user interface, particularly an XY pad, allowing users to intuitively manipulate the audio signal. Moving the cursor across the pad lets users vary the delay time for each channel, adding depth and spatiality to the sound.

### Signal Processing

The core of signal processing occurs in the plugin’s `processBlock` function. Here, the audio signal is processed in real-time, with the delay time for each channel being calculated based on the current position on the XY pad. The plugin manages delay (`delayTime`) and the mix between the original and processed signals (`dryWetMix`), offering extensive control over the final effect.

#### Delay Calculation

The `getChannelSpecificDelayTime` function is key to determining the delay time for the left and right channels. The plugin treats negative delay values for the left channel and positive values for the right. This unique approach means that moving the cursor on the XY pad simultaneously reduces delay on one channel while increasing it on the other. The actual delay time is calculated as a fraction of the maximum delay time (`maxDelayTimeMs`), proportional to the cursor's position on the pad.

### User Interface

The plugin's user interface is built using the `PluginEditor` and `XyPad` classes, providing a visual and interactive interface. The XY pad, implemented in the `XyPad` class, is central to user interaction, allowing immediate and intuitive control over the delay effects.

## Conclusion

The XY Pad Audio plugin offers a unique and powerful way to manipulate sound, providing users with direct control over complex stereo effects. Its combination of sophisticated audio signal processing and an intuitive user interface makes it an excellent tool for music producers, sound engineers, and audio enthusiasts eager to explore new sonic dimensions.
