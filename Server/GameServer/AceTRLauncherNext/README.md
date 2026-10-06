# AceTR Launcher Next

Sıfırdan yazılmış modern WPF launcher kabuğu.

## Teknoloji
- .NET 8 WPF (x86)
- Microsoft Edge WebView2
- Tamamen özel skin/hover/input/checkbox katmanı
- Eski AtumLauncher backend'i ile çevre değişkenleri üzerinden güvenli entegrasyon için hazır köprü

## Tasarım asset'i
`Assets/launcher_master.jpg` dosyası gereklidir. Tasarım 1636x930 piksel baz alınmıştır.

## launcher.json
Home/Discord/Facebook/Destek URL'leri ve backend executable burada tanımlanır.

## Backend protokolü
Yeni launcher girişte eski backend'i şu environment değerleriyle başlatır:
- ACETR_MODERN_LAUNCHER=1
- ACETR_ACCOUNT
- ACETR_PASSWORD
- ACETR_REMEMBER
- ACETR_64BIT
- ACETR_WINDOWED

Bir sonraki backend adımı mevcut C++ AtumLauncher'ın bu değerleri okuyup görünmeden update + PreServer login + game launch yapmasıdır.
