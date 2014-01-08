=========
Tethering
=========

A tethering GUI for the `Jolla`_ phone (`SailfishOS`_). It supports Bluetooth and
Wifi (WPA2) tethering by controlling `ConnMan`_ directly via D-BUS. The GUI will
automatically create a packet data connection and power on the necessary
interface(s), when enabling tethering. It will also restore the states of
the interfaces, when a user disables tethering.

.. _`Jolla`: http://jolla.com
.. _`SailfishOS`: http://sailfishos.org
.. _`ConnMan`: http://connman.net

At the moment only basic information is shown in the GUI. This is somewhat
due to the fact that ConnMan doesn't (yet) provide detailed information about
tethering. The same applies to the configurable parameters as ConnMan only
allows for configuring the SSID and PSK of the Wifi network. For example the
private network address is randomized every time and cannot be explicitly
configured.

Installable RPM packages can be downloaded from the pkg directory. Also
available in Jolla store.


License
=======

Everything is released under the terms of BSD-new.


Screenshots
===========

.. image:: screenshots/app_131230.png


Known Issues/Notes
==================

* In some cases the enabling process might end up into an error and a user
  will need to try again or restart the app and try again.

    * Keep in mind though that not all errors are bugs in the gui


* Any feedback and/or contributions are always welcome


Releases
========

* Features:
    * Wifi tethering
    * Bluetooth tethering
    * Automatic connection handling
    * Restoring states of the interfaces

* Status:
    * ``Beta``

* Changelog:
    * ``v0.2.7``
        * fix issues due to Maadajävri's auto connect

            - fix enabling of tethering, when there's an active wifi connection
            - keep wifi switched off during bluetooth tethering
        * fix an issue with enabling of bluetooth tethering, where the gui was
          sometimes left into "Enabling..." state
        * keep record of mobile data auto connect state
    * ``v0.2.5``
        * harbourize
    * ``v0.2.4``
        * first release

