# Guardfile
# More info at https://github.com/guard/guard#readme

notification :off

group :development do

  # Only run Compass if we have a config.rb file in place.
  if File.exists?("config.rb")
    # Compile on start.
    puts `compass compile --time --quiet`

    # https://github.com/guard/guard-compass
    guard :compass do
      watch(%r{.+\.s[ac]ss$})
    end
  end

  # https://github.com/guard/guard-livereload.
  guard :livereload do
    watch(%r{.+\.(css|js)$})
  end

end
