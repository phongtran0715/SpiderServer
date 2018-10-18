I. Install Spider server
    1. Install omniORB from link: http://omniorb.sourceforge.net/
    2. Prerequire package
        sudo apt-get update
        sudo apt-get upgrade
        sudo apt-get install build-essential python-dev python-setuptools python-pip python-smbus
        sudo apt-get install libssh-dev libcurl4-openssl-dev
        sudo apt-get install libmysqlclient-dev 
        sudo apt install libmosquitto-dev mosquitto
        #option package
        sudo apt-get install libncursesw5-dev libgdbm-dev libc6-dev
        sudo apt-get install zlib1g-dev libsqlite3-dev tk-dev
        sudo apt-get install libssl-dev openssl
        sudo apt-get install libffi-dev
    3. Configure & install
        ./configure --with-server --with-agent --with-mysql --prefix=/opt/netxms/
        make
        sudo make install
    4. Init database
        - Run init database for netxms
        - Run init database for spiderboot (spiderboot.sql)

II. Install youtube-dl
    https://www.howtoforge.com/tutorial/install-and-use-youtube-dl-on-ubuntu-1604/

III. Install ffmpeg
    - Download ffmpeg static build package from : https://johnvansickle.com/ffmpeg/
    - Create ffmpeg alias (append this content to ~./bashrc file)
            
        export OMNIORB_CONFIG=/opt/omniORB/sample.cfg
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
        export FFMPEG=__REPLACE_BY_FFMPEG_PATH__"/ffmpeg"
        export FFPROBE=__REPLACE_BY_FFMPEG_PATH__"/ffprobe"
        alias ffmpeg=__REPLACE_BY_FFMPEG_PATH__/ffmpeg

     - Apply change by run command     
        #source ~/.bashrc

IV. Config omniname
    - Note: 
        Option 1: turn off file wall to make sure pass connection between two server instance
        Option 2: Create fire rule 
            https://www.digitalocean.com/community/tutorials/how-to-set-up-a-firewall-with-ufw-on-ubuntu-16-04
V. Configuration file template
    #List config file in /etc/spider
        - D001.config
        - R001.config
        - U001.config
        - YoutubeAgent.config
        - download_log4j.properties
        - render_log4j.properties
        - upload_log4j.properties
        - youtubeAgent_log4j.properties
    1. Download app config
        ############################################################        
        APIKey = AIzaSyDGGgByVC_2oIOaqznVM03GSpIfb5Ghyuc
        ClientSecret = 
        AppId = D001_local
        OutputVideo = /home/phongtran0715/Downloads/Video/SpiderData
        VideoFormat = mp4
        MaxResult = 3
        CorbaRef = NameService=corbaloc::127.0.0.1:2809/NameService
        ############################################################
    2. Render app config
        ############################################################
        AppId = R001_local
        OutputVideo = /home/phongtran0715/Downloads/Video/SpiderData/render_output
        VideoFormat = mp4
        CorbaRef = NameService=corbaloc::127.0.0.1:2809/NameService
        IP = 127.0.0.1
        ############################################################
    3. Upload app config
        ############################################################
        AppId = U001_local
        UploadDelayTime = 10800
        CorbaRef = NameService=corbaloc::127.0.0.1:2809/NameService
        Ip = 127.0.0.1
        ############################################################
    4. log4j properties config
        - 

