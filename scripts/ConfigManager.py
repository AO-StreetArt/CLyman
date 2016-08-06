#A configuration manager for the application
class ConfigurationManager():

    def __init__(self):
        self.param_list = {}

    #Custom Container methods
    def __len__(self):
        return len(self.param_list)

    def __getitem__(self, key):
        return self.param_list[key]

    def __setitem__(self, key, value):
        self.param_list[key] = value

    def __delitem__(self, key):
        del self.param_list[key]

    def __iter__(self):
        return iter(self.param_list)

    def configure(self, config_file):
        #Parse the config file and pull the values
        with open(config_file, 'r') as f:
            for line in f:
                line = line.rstrip() #removes trailing whitespace and '\n' chars

                if "=" not in line: continue #skips blanks and comments w/o =
                if line.startswith("#"): continue #skips comments which contain =

                k, v = line.split("=", 1)
                self.param_list[k] = v
