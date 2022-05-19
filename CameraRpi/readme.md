## Setup

Setup a rpi **with desktop** and recommended software via rpi Imager

## Post Setup

 * copy over restart.py
 * Add the following to crontab 
   * `@reboot sleep 15 && DISPLAY=:0.0 python restart.py`

