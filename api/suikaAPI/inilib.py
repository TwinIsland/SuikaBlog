class SimpleIniParser:
    def __init__(self):
        self.config = {}

    def parse(self, filepath):
        """Parse an INI file and store the results in a dictionary."""
        current_section = None
        with open(filepath, 'r') as file:
            for line in file:
                line = line.strip()
                # Skip blank lines and comments
                if not line or line.startswith(';') or line.startswith('#'):
                    continue
                # Section titles [section]
                if line.startswith('[') and line.endswith(']'):
                    current_section = line[1:-1]
                    self.config[current_section] = {}
                # Key-value pairs key=value
                elif '=' in line and current_section is not None:
                    key, value = line.split('=', 1)
                    key, value = key.strip(), value.strip()
                    self.config[current_section][key] = value
                else:
                    raise ValueError(f"Line not recognized: {line}")

    def get(self, section, key, default=None):
        """Retrieve a value from the configuration."""
        return self.config.get(section, {}).get(key, default)
