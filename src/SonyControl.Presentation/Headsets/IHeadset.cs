namespace SonyControl.Presentation.Headsets;

/// <summary>
/// One Sony headset's control link.
/// </summary>
/// <remarks>
/// Every command completes once the headset acknowledges it, and <see cref="State"/> only
/// changes after that, so callers can revert their UI to <see cref="State"/> when a command fails.
/// Events are raised on a background thread.
/// </remarks>
public interface IHeadset : IDisposable
{
    string DeviceName { get; }

    string ModelName { get; }

    bool IsKnownModel { get; }

    HeadsetFeatures Features { get; }

    HeadsetSnapshot State { get; }

    IReadOnlyList<EqualizerPresetOption> EqualizerPresets { get; }

    event EventHandler<HeadsetSnapshot>? StateChanged;

    event EventHandler? Disconnected;

    Task ConnectAsync(string bluetoothAddress);

    void Disconnect();

    Task RefreshBatteryAsync();

    Task SetNoiseControlAsync(NoiseControlSetting value);

    Task SetEqualizerPresetAsync(int preset);

    Task SetEqualizerCustomAsync(EqualizerSetting value);

    Task SetConnectionQualityAsync(bool prioritizeStableConnection) =>
        Task.FromException(new NotSupportedException("Connection quality is not supported by this headset."));

    Task SetDseeAsync(bool enabled);

    Task SetVptAsync(int preset) =>
        Task.FromException(new NotSupportedException("VPT is not supported by this headset."));

    Task SetSoundPositionAsync(int position) =>
        Task.FromException(new NotSupportedException("Sound position is not supported by this headset."));

    Task SetVoiceGuidanceAsync(int value) =>
        Task.FromException(new NotSupportedException("Voice guidance is not supported by this headset."));

    /// <summary>
    /// Turns the headset off. The control link drops afterwards.
    /// </summary>
    Task PowerOffAsync();

    Task SetSpeakToChatAsync(bool enabled);

    Task SetAdaptiveVolumeAsync(bool enabled);

    Task SetAutoPowerOffAsync(int index);

    /// <summary>
    /// Multipoint: moves playback to the connected device with this address. Fails when the
    /// headset refuses (on a call, say).
    /// </summary>
    Task SwitchPlaybackAsync(string address);
}
