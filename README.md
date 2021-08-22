# ASBEAR (Another Simple But Efficient Audio Recorder)
ASBEAR is a simple audio recorder made with Qt 6.1.2 and SFML 2.5.1,
but not for too long, I will go back to Qt 5 soon for snap packaging...

The application has been renamed because other software use MRecorder
I'm rewriting it in order to port it to Linux where it will be available in the snap store and I'm trying
to make the code still compatible with Windows but I won't build for Windows anymore.
The interface and the use of the app will not change a lot, I'm just optimizing the code and reworking the files hierarchy (it is kind of a mess).
The user data will now be stored in appdata folders : AppData/Local over Windows and ~/.config over Linux.
