class PAX
  Network = ::Network

  class Display
    SLOTS_AVAILABLES = 8
    def self.clear
      PAX.display_clear
    end

    def self.clear_line(line)
      PAX.display_clear_line(line)
    end

    def self.display_bitmap(path, row = 0, column = 0)
      if File.exists? path
        PAX.print_bitmap(path, row, column)
      end
    end

    def self.print_in_line(buf, row=0, column=0)
      __print__(buf, row, column)
    end

    def self.status_bar_slots_available
      SLOTS_AVAILABLES
    end

    def self.main_image
      "main_#{PAX::System.model}.bmp"
    end
  end

  def self.set_os_values
    ret = PAX::System.install("MAINAPP", "./shared/keymap.dat", PAX::System::FILE_TYPE_APP_PARAM) if File.exists?("./shared/keymap.dat")
    PAX::System._os_set_value("persist.sys.mainapp.restart", "1")
    PAX::System._os_set_value("persist.sys.sound.enable", "true")
    PAX::System._os_set_value("persist.sys.backlighttime", 180.chr)
    PAX::System._os_set_value("persist.sys.continue.print", "1")
    PAX::Audio.volume = 5
  end

  def self.define_device_modules
    Device.const_set(:Touch             , PAX::Touch)
    Device.const_set(:Pinpad            , EMVPlatform::EMV::Pinpad)
    Device.const_set(:SamCard           , PAX::SamCard)
    Device.const_set(:MifareCard        , PAX::MifareCard)
    Device.const_set(:EMV               , EMVPlatform::EMV)
    Device::Crypto.const_set(:Newdes    , PAX::Crypto::Newdes)
    Device::Network.const_set(:Wifi     , PAX::Network::Wifi)
    Device::Network.const_set(:Gprs     , PAX::Network::Gprs)
    Device::Network.const_set(:Ethernet , PAX::Network::Ethernet)
    Device::Network.const_set(:Wifi     , PAX::Network::Wifi)
  end

  def self.pagination_keys
    {
      "d200" => { :back_key => Device::IO::F1   , :back_key_label => " F1 " , :forward_key => Device::IO::F2    , :forward_key_label => " F2 " },
      "d195" => { :back_key => Device::IO::F1   , :back_key_label => " F1 " , :forward_key => Device::IO::F2    , :forward_key_label => " F2 " },
      "s920" => { :back_key => Device::IO::FUNC , :back_key_label => "PRT " , :forward_key => Device::IO::ALPHA , :forward_key_label => "MENU"}
    }
  end

  def self.screen_definition
    case PAX::System.model
    when "d200", "d195"
      [22, 8]
    when "s920"
      [22, 14]
    else
      [22, 8]
    end
  end

  def self.set_keyboard
    if keys = self.pagination_keys[PAX::System.model]
      keyboard = ["qzQZ.", "abcABC", "defDEF", "ghiGHI", "jklJKL", "mnoMNO",
                  "prsPRS", "tuvTUV", "wxyWXY", ', */\{}[]#_$%-+=!?@&$']
      Device::IO.setup_keyboard(keyboard, keys)
    end
  end

  def self.printer_start
    Device::Printer.start
    PAX::Printer.font("AerialMono.ttf")
  end

  def self.setup
    Screen.setup(*screen_definition)
    begin
      define_device_modules
      require 'posxml_parser'
      require 'cloudwalk_handshake'
      CloudwalkHandshake.configure!
      self.set_os_values
      self.printer_start
      self.set_keyboard
    rescue LoadError => e
      ContextLog.exception(e, e.backtrace)
    rescue NameError => e
      ContextLog.exception(e, e.backtrace)
    end
  end
end

Platform = PAX

