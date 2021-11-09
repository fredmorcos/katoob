# TODO List for Katoob 0.7.0

- Since it's now using `GtkApplication`, supported command-line arguments need to be
  registered in the Application's constructor. Single-instance will also be problematic,
  since Katoob supports a feature to open a new document with the contents from
  stdin. This can be achieved by the second instance creating a tempfile with the contents
  of stdin and sending the primary instance the signal to open that tempfile.

  For general information about the application, the signals and command-line arguments
  handling, see: https://developer.gnome.org/documentation/tutorials/application.html
