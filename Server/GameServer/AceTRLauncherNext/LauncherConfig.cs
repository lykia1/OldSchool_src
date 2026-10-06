using System;
using System.Collections.Generic;
using System.IO;

namespace AceTRLauncher
{
    public sealed class LauncherConfig
    {
        public string HomeUrl = "https://example.com/";
        public string DiscordUrl = "";
        public string FacebookUrl = "";
        public string SupportUrl = "";
        public string BackendExecutable = "AtumLauncher.exe";
        public string ServerName = "AceTR - Ana Sunucu";

        public static LauncherConfig Load(string path)
        {
            var cfg = new LauncherConfig();
            if (!File.Exists(path)) return cfg;

            var values = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            foreach (var raw in File.ReadAllLines(path))
            {
                var line = raw.Trim();
                if (line.Length == 0 || line.StartsWith(";") || line.StartsWith("#") || line.StartsWith("["))
                    continue;
                var eq = line.IndexOf('=');
                if (eq <= 0) continue;
                values[line.Substring(0, eq).Trim()] = line.Substring(eq + 1).Trim();
            }

            string v;
            if (values.TryGetValue("HomeUrl", out v)) cfg.HomeUrl = v;
            if (values.TryGetValue("DiscordUrl", out v)) cfg.DiscordUrl = v;
            if (values.TryGetValue("FacebookUrl", out v)) cfg.FacebookUrl = v;
            if (values.TryGetValue("SupportUrl", out v)) cfg.SupportUrl = v;
            if (values.TryGetValue("BackendExecutable", out v)) cfg.BackendExecutable = v;
            if (values.TryGetValue("ServerName", out v)) cfg.ServerName = v;
            return cfg;
        }
    }
}
