class PAX
  Network = ::Network

  class Audio
    def self.beep(tone, milliseconds)
      PAX.beep(tone, milliseconds)
    end
  end

  class Display
    def self.clear
      PAX.display_clear
    end

    def self.clear_line(line)
      PAX.display_clear_line(line)
    end

    def self.display_bitmap(path, row = 0, column = 0)
      PAX.print_bitmap(path, row, column)
    end

    def self.print_in_line(buf, row=0, column=0)
      __print__(buf, row, column)
    end
  end

  class System
    DEFAULT_BACKLIGHT = 1
    def self.serial
      PAX._serial
    end

    def self.backlight=(level)
      PAX._backlight = level
    end

    def self.backlight
      DEFAULT_BACKLIGHT
    end

    def self.battery
      PAX._battery
    end

    def self.reboot
      PAX._reboot
    end

    class << self
      alias_method :restart, :reboot
    end
  end

  def self.setup
    Screen.setup(20, 7)
    begin
      require 'cloudwalk_handshake'
      CloudwalkHandshake.configure!
    rescue LoadError
    rescue NameError
    end
  end
end

