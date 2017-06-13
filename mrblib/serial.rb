class PAX
  class Serial
    COMS = [
      "/dev/ttyAMA1", # no OsWlinit
      "/dev/ttyACM5", # no OsWlinit, H330S
      "/dev/mux1",    # OsWlinit OK
    ]

    BAUD_1200   = 1200
    BAUD_2400   = 2400
    BAUD_4800   = 4800
    BAUD_9600   = 9600
    BAUD_19200  = 19200
    BAUD_38400  = 38400
    BAUD_57600  = 57600
    BAUD_115200 = 115200

    DATA_BITS_7 = 7
    DATA_BITS_8 = 8

    STOPBITS_1  = 1
    STOPBITS_2  = 2

    PARITY_E    = "E"
    PARITY_N    = "N"
    PARITY_O    = "O"

    DEFAULT_TIMEOUT = 3000

    attr_reader :fd, :com, :speed, :databits, :parity, :stopbits
    attr_accessor :timeout

    def initialize(com, speed, databits, parity, stopbits, timeout = DEFAULT_TIMEOUT)
      @com      = com
      @speed    = speed
      @databits = databits
      @parity   = parity
      @stopbits = stopbits
      @timeout  = timeout
      open(com, speed, databits, parity, stopbits)
    end

    def read(eot = "\n")
      str = ""
      timeout = Time.now + DEFAULT_TIMEOUT / 1000
      while(ch = recv(200))
        str << ch
        break if str.include?(eot)
        break if timeout < Time.now
      end
      str
    end

    def command(str)
      send(str)
      read
    end
  end
end
