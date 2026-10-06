
// 2005-04-28 by cmkwon
// #include "StringDefineServer.h"
#ifndef _STRING_DEFINE_SERVER_H_
#define _STRING_DEFINE_SERVER_H_

#include "StringDefineCommon.h"


///////////////////////////////////////////////////////////////////////////////
// 1 Atum
	// 1-1 
	#define STRERR_S_ATUMEXE_0001 "Sunucu etkin degil. 
Lutfen sag tiklayip -> Yonetici olarak calistirin !"
	#define STRERR_S_ATUMEXE_0002 "Soket Pre Server tarafindan kapatildi!"
	#define STRERR_S_ATUMEXE_0003 "Otomatik guncelleme basarisiz.
Lutfen oyunu yeniden kurun.
"
	#define STRERR_S_ATUMEXE_0004 "%s[%s] kaynagindan HATA %s(%#04X) alindi
"
	#define STRERR_S_ATUMEXE_0005 "Bilinmeyen Hata: %s(%#04x)"
	#define STRERR_S_ATUMEXE_0006 "Indirme sunucusuna baglanilamiyor."
	#define STRERR_S_ATUMEXE_0007 "Indirilecek dosyalarin boyutu bilinmiyor."
	#define STRERR_S_ATUMEXE_0008 "Guncelleme dosyasi indirilemiyor."
	#define STRERR_S_ATUMEXE_0009 "Sistemde yeterli bellek veya kaynak yok."
	#define STRERR_S_ATUMEXE_0010 ".exe dosyasi gecersiz."
	#define STRERR_S_ATUMEXE_0011 "Dosya bulunamadi."
	#define STRERR_S_ATUMEXE_0012 "Belirtilen yol bulunamadi. "
// 2006-04-20 by cmkwon, 	#define STRERR_S_ATUMEXE_0013 "[Error]Unknown Message Type: %d(0x%08X)\n"
// 1_end
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 2 - AtumLauncher
	// 2-1 STRMSG
	#define STRMSG_S_ATUMLAUNCHER_0000 "Guncelleme tamamlandi"
	#define STRMSG_S_ATUMLAUNCHER_0001 "Lutfen bir sunucu secin."
	#define STRMSG_S_ATUMLAUNCHER_0002 "Silinecek Dosya Listesi guncelleniyor v%s
"
	#define STRMSG_S_ATUMLAUNCHER_0003 "Gecici dosyalar siliniyor"
	#define STRMSG_S_ATUMLAUNCHER_0004 "Duyuru guncelleniyor"
	#define STRMSG_S_ATUMLAUNCHER_0005 "Guncelleme tamamlandi(%s -> %s)"
	#define STRMSG_S_ATUMLAUNCHER_0006 "Islem dosyasini secin"
	#define STRMSG_S_ATUMLAUNCHER_0007 "Islem yolunu secin"
	#define STRMSG_S_ATUMLAUNCHER_0008 "Indirme iptal edildi"
	#define STRMSG_S_ATUMLAUNCHER_0009 "Indirme tamamlandi"
	#define STRMSG_S_ATUMLAUNCHER_0010 "v%s surumune guncelleniyor - %s(%d/%d)"
	#define STRMSG_S_ATUMLAUNCHER_0011 "%s dosyasi olusturulamadi"
	#define STRMSG_S_ATUMLAUNCHER_0012 "v%s surumune guncelleniyor - %s(%d/%d)"
	#define STRMSG_S_ATUMLAUNCHER_0013 "Dosya Bilgisi Aliniyor %s"
	
	// STRERR
	#define STRERR_S_ATUMLAUNCHER_0000 "[Hata] Parametre Sayisi Hatasi, Sayi(%d)
"
	#define STRERR_S_ATUMLAUNCHER_0001 "[Hata] Mutex Hatasi
"
	#define STRERR_S_ATUMLAUNCHER_0002 "[Hata] Calistirma Turu Hatasi, Tur(%s)
"
	#define STRERR_S_ATUMLAUNCHER_0003 "[Hata] Sifre Cozme ID Hatasi, CozulenID(%s)
"
	#define STRERR_S_ATUMLAUNCHER_0004 "Pre Server'a baglanilamiyor."
	#define STRERR_S_ATUMLAUNCHER_0005 "Sunucuya baglanti basarisiz."
	#define STRERR_S_ATUMLAUNCHER_0006 "Soket Pre Server tarafindan kapatildi!"
	#define STRERR_S_ATUMLAUNCHER_0007 "Duyuru Dosyasi Hatasi!"
	#define STRERR_S_ATUMLAUNCHER_0009 "Otomatik guncelleme basarisiz.
Lutfen oyunu yeniden kurun.
"
	#define STRERR_S_ATUMLAUNCHER_0010 "Su anda tum sunucular devre disi."
	#define STRERR_S_ATUMLAUNCHER_0011 "%-16s%s bakimda..."
	#define STRERR_S_ATUMLAUNCHER_0012 "Tum sunucular bakimda. Daha sonra tekrar giris yapin."
	#define STRERR_S_ATUMLAUNCHER_0013 "%s[%s] kaynagindan HATA %s(%#04X) alindi
"
// 2006-05-26 by cmkwon	#define STRERR_S_ATUMLAUNCHER_0014 "Wrong ID, password error\n\n* only people who are certified as beta tester can log in at the present."
//	#define STRERR_S_ATUMLAUNCHER_0014 "You have entered an invalid login ID or password.  Please enter a registered login ID and password."
	#define STRERR_S_ATUMLAUNCHER_0015 "Giris islemi hatasi"
	#define STRERR_S_ATUMLAUNCHER_0016 "Kullanici ID girilmedi"
	#define STRERR_S_ATUMLAUNCHER_0017 "Cift giris"
	#define STRERR_S_ATUMLAUNCHER_0018 "F sunucusu calismiyor."
	#define STRERR_S_ATUMLAUNCHER_0019 "I sunucusu calismiyor."
	#define STRERR_S_ATUMLAUNCHER_0020 "Hizmet gecici olarak durduruldu.

Daha sonra tekrar giris yapin."
	#define STRERR_S_ATUMLAUNCHER_0021 "Cok fazla kullanici cevrimici.

Daha sonra tekrar giris yapin."
	#define STRERR_S_ATUMLAUNCHER_0022 "Hesabiniz su anda engelli.
Sure : %s

Ayrintili bilgi icin Musteri Destegi ile iletisime gecin: [https://support.oldschoolrivals.com/]"
	#define STRERR_S_ATUMLAUNCHER_0023 "Istemci surumu hatali.

Lutfen oyunu yeniden indirin."
	#define STRERR_S_ATUMLAUNCHER_0024 "HATA: %s(%#04X)"
	#define STRERR_S_ATUMLAUNCHER_0025 "Indirme sunucusuna giris yapilamiyor."
	#define STRERR_S_ATUMLAUNCHER_0026 "Indirilecek dosyanin boyutu(%s) belirlenemiyor."
	#define STRERR_S_ATUMLAUNCHER_0027 "Indirilecek dosyanin boyutu bilinmiyor."
	#define STRERR_S_ATUMLAUNCHER_0028 "Guncelleme dosyasi indirilemiyor."
	#define STRERR_S_ATUMLAUNCHER_0029 "Guncellenen dosya bulunmuyor."
	#define STRERR_S_ATUMLAUNCHER_0030 "Secilen sunucu bakimda. Daha sonra tekrar giris yapin."
	#define STRERR_S_ATUMLAUNCHER_0031 "Sistemde yeterli bellek veya kaynak yok."
	#define STRERR_S_ATUMLAUNCHER_0032 ".exe dosyasi gecersiz."
	#define STRERR_S_ATUMLAUNCHER_0033 "Dosya bulunamiyor."
	#define STRERR_S_ATUMLAUNCHER_0034 "Yol bulunamiyor."
// 2006-04-20 by cmkwon	#define STRERR_S_ATUMLAUNCHER_0035 "[Error] Unhandled Message Type: %s(%#04X)\n"
// 2006-04-20 by cmkwon	#define STRERR_S_ATUMLAUNCHER_0036 "[Error] Unhandled Message Type!\n"

	#define STRMSG_S_050506		"'%s' hesabi su anda engelli.
  Neden: %s
  Sure: %s~%s

Ayrintili bilgi icin Musteri Destegi ile iletisime gecin: [https://support.oldschoolrivals.com]"
	#define STRMSG_S_050930		"Lutfen oyunu yeniden indirin.
URL: [https://oldschoolrivals.com] %s
En yeni surum: "
// 2_end
///////////////////////////////////////////////////////////////////////////////	

///////////////////////////////////////////////////////////////////////////////
// 3 - AtumAdminTool
	// 3-1 STRMSG
//	#define STRMSG_S_SCADMINTOOL_0000 "Male"
//	#define STRMSG_S_SCADMINTOOL_0001 "Female"
//	#define STRMSG_S_SCADMINTOOL_0002 "A.D%d, Age%d"
//	#define STRMSG_S_SCADMINTOOL_0003 "Do you really want to modify your account information?"
//	#define STRMSG_S_SCADMINTOOL_0004 "CAST(l.CurrentCount AS VARCHAR(10)) + 'piece'"
//	#define STRMSG_S_SCADMINTOOL_0005 "CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces attained, ' + CAST(l.CurrentCount AS VARCHAR(10)) + 'piece'"
//	#define STRMSG_S_SCADMINTOOL_0006 "'''To ' + l.PeerCharacterName + ''', give ' + CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces, total ' + CAST(l.CurrentCount AS VARCHAR(10)) + 'pieces'"
//	#define STRMSG_S_SCADMINTOOL_0007 "'''To ' + l.PeerCharacterName + ''', receive ' + CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces, total ' + CAST(l.CurrentCount AS VARCHAR(10)) + 'pieces'"
//	#define STRMSG_S_SCADMINTOOL_0008 "'''Discard ' + CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces, total ' + CAST(l.CurrentCount AS VARCHAR(10)) + 'pieces'"
//	#define STRMSG_S_SCADMINTOOL_0009 "CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces bought, remaining Spi: ' + CAST(l.RemainedMoney AS VARCHAR(10))"
//	#define STRMSG_S_SCADMINTOOL_0010 "CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces sold, remaining Spi: ' + CAST(l.RemainedMoney AS VARCHAR(10))"
//	#define STRMSG_S_SCADMINTOOL_0011 "CAST(l.CurrentCount AS VARCHAR(10)) + 'pieces'"
	#define STRMSG_S_SCADMINTOOL_0012 "'''' + l.PeerCharacterName + ''' added ' + CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces, total ' + CAST(l.CurrentCount AS VARCHAR(10)) + 'pieces'"
	#define STRMSG_S_SCADMINTOOL_0013 "'''' + l.PeerCharacterName + ''' deleted ' + CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces, total ' + CAST(l.CurrentCount AS VARCHAR(10)) + 'piece'"
	#define STRMSG_S_SCADMINTOOL_0014 "'Add ' + CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces , ' + CAST(l.CurrentCount AS VARCHAR(10)) + 'pieces'"
	#define STRMSG_S_SCADMINTOOL_0015 "'Deposit ' + CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces, total ' + CAST(l.CurrentCount AS VARCHAR(10)) + 'pieces'"
	#define STRMSG_S_SCADMINTOOL_0016 "'Recover ' + CAST(l.ChangeCount AS VARCHAR(10)) + 'pieces, total' + CAST(l.CurrentCount AS VARCHAR(10)) + 'pieces'"
	#define STRMSG_S_SCADMINTOOL_0017 "\'game time \' + dbo.atum_GetHMSFromS(l.PlayTime) + \', whole \' + dbo.atum_GetHMSFromS(l.TotalPlayTime)"
	#define STRMSG_S_SCADMINTOOL_0018 "CAST(l.Param1 AS VARCHAR(10)) + ' -> ' + CAST(l.Param2 AS VARCHAR(10)) + ', game time: ' + dbo.atum_GetHMSFromS(l.Param3)"
//	#define STRMSG_S_SCADMINTOOL_0019 "CAST(l.Param1 AS VARCHAR(15)) + \' rise and fall, whole \' + CAST(l.Param2 AS VARCHAR(15))"
//	#define STRMSG_S_SCADMINTOOL_0020 "Crash"
//	#define STRMSG_S_SCADMINTOOL_0021 "Monster"
//	#define STRMSG_S_SCADMINTOOL_0022 "GEAR"
//	#define STRMSG_S_SCADMINTOOL_0023 "Reason unknown"
//	#define STRMSG_S_SCADMINTOOL_0024 "%s, Remaining stat: %s"
	#define STRMSG_S_SCADMINTOOL_0025 "(Mevcut degil)"
//	#define STRMSG_S_SCADMINTOOL_0026 "myself"
//	#define STRMSG_S_SCADMINTOOL_0027 "Does not exist"
//	#define STRMSG_S_SCADMINTOOL_0028 "User with bug use"
	#define STRMSG_S_SCADMINTOOL_0029 "Hesap engelli"
	#define STRMSG_S_SCADMINTOOL_0030 "Sohbet yasakli"
//	#define STRMSG_S_SCADMINTOOL_0031 "Connection log"
//	#define STRMSG_S_SCADMINTOOL_0032 "User log"
//	#define STRMSG_S_SCADMINTOOL_0033 "Item log"
	#define STRMSG_S_SCADMINTOOL_0034 "%s - %s sunucusu"
	#define STRMSG_S_SCADMINTOOL_0035 "%s - %s sunucusu,%d(%d)"
	#define STRMSG_S_SCADMINTOOL_0036 " Hesaba el konuldu"
//	#define STRMSG_S_SCADMINTOOL_0037 "Classification    "
//	#define STRMSG_S_SCADMINTOOL_0038 "Value"
	#define STRMSG_S_SCADMINTOOL_0039 "Bu hesabin engelini kaldirmak istiyor musunuz?"
	#define STRMSG_S_SCADMINTOOL_0040 "%s(%dsaniye)"
	#define STRMSG_S_SCADMINTOOL_0041 "%dgalibiyet %dmaglubiyet"
	#define STRMSG_S_SCADMINTOOL_0042 "Baglantiyi kesip hesabi engellemek istediginize emin misiniz?"
	#define STRMSG_S_SCADMINTOOL_0043 "%s esyasi"
	#define STRMSG_S_SCADMINTOOL_0044 "[%s %15s] Duyuru : %s
"
	#define STRMSG_S_SCADMINTOOL_0045 "Kullanici sayisi : %d
"
	#define STRMSG_S_SCADMINTOOL_0046 "[%s %15s] Alinan mesaj : %s
"
	#define STRMSG_S_SCADMINTOOL_0047 "[%s %15s] kullanici sayisi : %4d
"
	#define STRMSG_S_SCADMINTOOL_0048 "[%s %15s] FieldServer durumu : %d
"
	#define STRMSG_S_SCADMINTOOL_0049 "Sunucu baglantisi kesildi
Soket Adi: %s
IP: %s"
	
	// 3-2 AtumAdminTool - STRERR
	#define STRERR_S_SCADMINTOOL_0000 "Ilgili hesap bilgisi yuklenemedi."
	#define STRERR_S_SCADMINTOOL_0001 "Hesap bilgisi degistirilemedi."
	#define STRERR_S_SCADMINTOOL_0002 "Sifrenizi girin"
	#define STRERR_S_SCADMINTOOL_0003 "Sifre dogrulamasi basarisiz"
	#define STRERR_S_SCADMINTOOL_0004 "Giris adini girin"
	#define STRERR_S_SCADMINTOOL_0005 "Pre Server calismiyor."
	#define STRERR_S_SCADMINTOOL_0006 "PreServer'a baglanilamiyor !!"
//	#define STRERR_S_SCADMINTOOL_0007 "인증 실패하였습니다"			// 2006-04-11 by cmkwon, 주석처리함
	#define STRERR_S_SCADMINTOOL_0008 "HATA: ilgili protokol bulunmuyor."
	#define STRERR_S_SCADMINTOOL_0009 "Kullanici adini girin"
	#define STRERR_S_SCADMINTOOL_0010 "Nedeni girin"
	#define STRERR_S_SCADMINTOOL_0011 "Deneyim puani ayar hatasi : Seviye %2d ==> Exp(%.1I64f ~ %.1I64f)"
	#define STRERR_S_SCADMINTOOL_0012 "Maksimum log sayisi asildi.

Lutfen maksimum log sayisini veya arama kosulunu degistirin."
	#define STRERR_S_SCADMINTOOL_0013 "Veritabanina baglanilamiyor."
	#define STRERR_S_SCADMINTOOL_0014 "Bagli degil"
	#define STRERR_S_SCADMINTOOL_0015 "Bagli"
	#define STRERR_S_SCADMINTOOL_0016 "Guncelleniyor"
	#define STRERR_S_SCADMINTOOL_0017 "Giris yapildi"
	#define STRERR_S_SCADMINTOOL_0018 "Karakter seciliyor"
	#define STRERR_S_SCADMINTOOL_0019 "Oyun oynaniyor"
	#define STRERR_S_SCADMINTOOL_0020 "Bilinmiyor"
	#define STRERR_S_SCADMINTOOL_0021 "Veritabani %s(%s:%d) baglantisi kurulamiyor."
	#define STRERR_S_SCADMINTOOL_0022 "Esya eklenemedi"
	#define STRERR_S_SCADMINTOOL_0023 "Engelli hesap bulunamadi."
	#define STRERR_S_SCADMINTOOL_0024 "Degistirmek icin once hesabinin baglantisini kesip engelleyin."
	#define STRERR_S_SCADMINTOOL_0025 "SPI(para) eklenemiyor."
	#define STRERR_S_SCADMINTOOL_0026 "Secili esya zaten mevcut, miktari degistirin."
	#define STRERR_S_SCADMINTOOL_0027 "Esya bulunurken hata"
	#define STRERR_S_SCADMINTOOL_0028 "Karakterin sahip oldugu SPI(para) silinemez."
	#define STRERR_S_SCADMINTOOL_0029 "Bu esyayi silmek istiyor musunuz?"
	#define STRERR_S_SCADMINTOOL_0030 "Esya silinemedi."
	#define STRERR_S_SCADMINTOOL_0031 "Esya degistirilemedi."
	#define STRERR_S_SCADMINTOOL_0032 "Hesabi girin"
	#define STRERR_S_SCADMINTOOL_0033 "Hesap bulunmuyor.(hesap engelli olabilir)"
	#define STRERR_S_SCADMINTOOL_0034 "Hesap veya karakter bulunmuyor."
	#define STRERR_S_SCADMINTOOL_0035 "Karakter bilgisi arama hatasi."
	#define STRERR_S_SCADMINTOOL_0036 "Ilgili karakter bulunmuyor"
	#define STRERR_S_SCADMINTOOL_0037 "Karakter guncelleme hatasi."
	#define STRERR_S_SCADMINTOOL_0038 "Karakter bilgisi basariyla guncellendi."
	#define STRERR_S_SCADMINTOOL_0039 "Bir esya secin."
	#define STRERR_S_SCADMINTOOL_0040 "Esya sayisini secin."
	#define STRERR_S_SCADMINTOOL_0041 "Ilgili esya 5 adetten az olusturulabilir."
	#define STRERR_S_SCADMINTOOL_0042 "Mesaji girin."
	#define STRERR_S_SCADMINTOOL_0043 "Duyuru gonderilemiyor."
	#define STRERR_S_SCADMINTOOL_0044 "%s: sunucu durumu(%d)
"
	#define STRERR_S_SCADMINTOOL_0045 "IM Server calismiyor."
	#define STRERR_S_SCADMINTOOL_0046 "Field Server calismiyor."
	#define STRERR_S_SCADMINTOOL_0047 "[%s %15s] IMServer durumu : %d
"

	// 3-3 AtumLaAtumAdminTool -
//	#define STRMSG_S_SCAT_COLNAME_0000 "Account name"
//	#define STRMSG_S_SCAT_COLNAME_0001 "Type"
//	#define STRMSG_S_SCAT_COLNAME_0002 "Conviction"
//	#define STRMSG_S_SCAT_COLNAME_0003 "Start date"
//	#define STRMSG_S_SCAT_COLNAME_0004 "Finish date"
//	#define STRMSG_S_SCAT_COLNAME_0005 "Handling person"
//	#define STRMSG_S_SCAT_COLNAME_0006 "Handling reason"
//	#define STRMSG_S_SCAT_COLNAME_0007 "Date"
//	#define STRMSG_S_SCAT_COLNAME_0008 "Log type"
//	#define STRMSG_S_SCAT_COLNAME_0009 "IPAddress"
//	#define STRMSG_S_SCAT_COLNAME_0010 "Server name"
//	#define STRMSG_S_SCAT_COLNAME_0011 "Character name"
//	#define STRMSG_S_SCAT_COLNAME_0012 "Location"
//	#define STRMSG_S_SCAT_COLNAME_0013 "Contents"
//	#define STRMSG_S_SCAT_COLNAME_0014 "Item"
//	#define STRMSG_S_SCAT_COLNAME_0015 "UID"
//	#define STRMSG_S_SCAT_COLNAME_0016 "Own number"
//	#define STRMSG_S_SCAT_COLNAME_0017 "Name"
//	#define STRMSG_S_SCAT_COLNAME_0018 "Item number"
//	#define STRMSG_S_SCAT_COLNAME_0019 "Prefix"
//	#define STRMSG_S_SCAT_COLNAME_0020 "Suffix"
	#define STRMSG_S_SCAT_COLNAME_0021 "Takili"
//	#define STRMSG_S_SCAT_COLNAME_0022 "Amount"
//	#define STRMSG_S_SCAT_COLNAME_0023 "Endurance"
//	#define STRMSG_S_SCAT_COLNAME_0024 "Create time"
	#define STRMSG_S_SCAT_COLNAME_0025 "Takili degil"
//	#define STRMSG_S_SCAT_COLNAME_0026 "Warehouse"
//	#define STRMSG_S_SCAT_COLNAME_0027 "Auction"
//	#define STRMSG_S_SCAT_COLNAME_0028 "Map"
//	#define STRMSG_S_SCAT_COLNAME_0029 "Coordinate"
//	#define STRMSG_S_SCAT_COLNAME_0030 "Sex"
//	#define STRMSG_S_SCAT_COLNAME_0031 "Race"
//	#define STRMSG_S_SCAT_COLNAME_0032 "Authority"
//	#define STRMSG_S_SCAT_COLNAME_0033 "Unit kind"
//	#define STRMSG_S_SCAT_COLNAME_0034 "Level"
//	#define STRMSG_S_SCAT_COLNAME_0035 "Maximum level"
//	#define STRMSG_S_SCAT_COLNAME_0036 "Experience point"
//	#define STRMSG_S_SCAT_COLNAME_0037 "Decreased experience point"
//	#define STRMSG_S_SCAT_COLNAME_0038 "Auto stat division type"
//	#define STRMSG_S_SCAT_COLNAME_0039 "Attack"
//	#define STRMSG_S_SCAT_COLNAME_0040 "Defense"
//	#define STRMSG_S_SCAT_COLNAME_0041 "Fuel"
//	#define STRMSG_S_SCAT_COLNAME_0042 "Spirit"
//	#define STRMSG_S_SCAT_COLNAME_0043 "Shield"
//	#define STRMSG_S_SCAT_COLNAME_0044 "Agility"
//	#define STRMSG_S_SCAT_COLNAME_0045 "Attached regiment"
//	#define STRMSG_S_SCAT_COLNAME_0046 "Propensity"
//	#define STRMSG_S_SCAT_COLNAME_0047 "social position"
//	#define STRMSG_S_SCAT_COLNAME_0048 "Whole connection time"
//	#define STRMSG_S_SCAT_COLNAME_0049 "Generated time"
//	#define STRMSG_S_SCAT_COLNAME_0050 "Final log in time"
//	#define STRMSG_S_SCAT_COLNAME_0051 "Type"
//	#define STRMSG_S_SCAT_COLNAME_0052 "Whole"
	#define STRMSG_S_SCAT_COLNAME_0053 "Otomatik tur(1-1tur)"
	#define STRMSG_S_SCAT_COLNAME_0054 "Vulkan turu(1-1tur)"	
	#define STRMSG_S_SCAT_COLNAME_0055 "Duellocu turu(1-1tur)" // 2005-08-01 by hblee : Grenade -> changed to dualist.
	#define STRMSG_S_SCAT_COLNAME_0056 "Top turu(1-1tur)"
	#define STRMSG_S_SCAT_COLNAME_0057 "Tufek turu(1-2tur)"
	#define STRMSG_S_SCAT_COLNAME_0058 "Gatling turu(1-2tur)"
	#define STRMSG_S_SCAT_COLNAME_0059 "Launcher type(1-2type)"
	#define STRMSG_S_SCAT_COLNAME_0060 "Mass drive type(1-2type)"
	#define STRMSG_S_SCAT_COLNAME_0061 "Rocket type(2-1type)"
	#define STRMSG_S_SCAT_COLNAME_0062 "Missile type(2-1type)"
	#define STRMSG_S_SCAT_COLNAME_0063 "Bundle type(2-1type)"
	#define STRMSG_S_SCAT_COLNAME_0064 "Mine type(2-1type)"
	#define STRMSG_S_SCAT_COLNAME_0065 "Shield type(2-2type)"
	#define STRMSG_S_SCAT_COLNAME_0066 "Dummy type(2-2type)"
	#define STRMSG_S_SCAT_COLNAME_0067 "Pixer type(2-2type)"
	#define STRMSG_S_SCAT_COLNAME_0068 "Decoy type(2-2type)"
	#define STRMSG_S_SCAT_COLNAME_0069 "Defense type"
	#define STRMSG_S_SCAT_COLNAME_0070 "Support equipment type"
	#define STRMSG_S_SCAT_COLNAME_0071 "Energy type"
	#define STRMSG_S_SCAT_COLNAME_0072 "Metal type"
	#define STRMSG_S_SCAT_COLNAME_0073 "Card type"
	#define STRMSG_S_SCAT_COLNAME_0074 "Enchant type"
	#define STRMSG_S_SCAT_COLNAME_0075 "Tank type"
	#define STRMSG_S_SCAT_COLNAME_0076 "Bullet type"
	#define STRMSG_S_SCAT_COLNAME_0077 "For quest"
	#define STRMSG_S_SCAT_COLNAME_0078 "Radar type"
	#define STRMSG_S_SCAT_COLNAME_0079 "Computer type"
	#define STRMSG_S_SCAT_COLNAME_0080 "Gamble Card type"
	#define STRMSG_S_SCAT_COLNAME_0081 "Enchant Destruction Prevention type"		// 2005-08-02 by cmkwon
	#define STRMSG_S_SCAT_COLNAME_0082 "Blaster type"				// 2005-08-02 by cmkwon
	#define STRMSG_S_SCAT_COLNAME_0083 "Rail gun type"				// 2005-08-02 by cmkwon
//	#define STRMSG_S_SCAT_COLNAME_0081 "Whole item"
//	#define STRMSG_S_SCAT_COLNAME_0082 "Whole server"
//	#define STRMSG_S_SCAT_COLNAME_0083 "Server name"
//	#define STRMSG_S_SCAT_COLNAME_0084 "Server type"
//	#define STRMSG_S_SCAT_COLNAME_0085 "Server IP, Port"
//	#define STRMSG_S_SCAT_COLNAME_0086 "Present number of users"
//	#define STRMSG_S_SCAT_COLNAME_0087 "Server condition"
//	#define STRMSG_S_SCAT_COLNAME_0088 "Group server"
//	#define STRMSG_S_SCAT_COLNAME_0089 "Field server"
//	#define STRMSG_S_SCAT_COLNAME_0090 "Unknown"
//	#define STRMSG_S_SCAT_COLNAME_0091 "Not executed"
//	#define STRMSG_S_SCAT_COLNAME_0092 "Normal"
//	#define STRMSG_S_SCAT_COLNAME_0093 "Abnormal"
	#define STRMSG_S_SCAT_COLNAME_0094 "Yonetici"

	#define STRMSG_S_SCADMINTOOL_050512_0000	"CAST(l.CurrentCount AS VARCHAR(10)) + '개, Param1:' + CAST(l.Param1 AS VARCHAR(10))"
// 3_end
///////////////////////////////////////////////////////////////////////////////
	
	
///////////////////////////////////////////////////////////////////////////////
// 4
	// 4-1 AtumMonitor - STRMSG
//	#define STRMSG_S_SCMONITOR_0000 "Command list\r\n"
//	#define STRMSG_S_SCMONITOR_0001 "----- List of backup account ---------------------------------------\r\n"
//	#define STRMSG_S_SCMONITOR_0002 "  Account: \'%s\', Original password: \'%s\', Temporary password: \'%s\'\r\n"
//	#define STRMSG_S_SCMONITOR_0003 "  Account: \'%s\' \r\n"
//	#define STRMSG_S_SCMONITOR_0004 "Please choose the folder where Versions for update are located"
//	#define STRMSG_S_SCMONITOR_0005 "\r\nMaking New Zip File From %s To %s...\r\n"
//	#define STRMSG_S_SCMONITOR_0006 "Rename Server Group"
//	#define STRMSG_S_SCMONITOR_0007 "File has been succesfully created.\r\n\r\nDB information: %s(%d), %s"
//	#define STRMSG_S_SCMONITOR_0008 "%04d(%2d -  active) %3d/%3d"
//	#define STRMSG_S_SCMONITOR_0009 "%04d(%2d -inactive) %3d/%3d"
//	#define STRMSG_S_SCMONITOR_0010 "Servers data has been succesfully reloaded."
//	#define STRMSG_S_SCMONITOR_0011 "Service condition has been succesfully reflected."
//	#define STRMSG_S_SCMONITOR_0012 "%04d(%2d -  active)"
//	#define STRMSG_S_SCMONITOR_0013 "%04d(%2d -inactive)"
//	#define STRMSG_S_SCMONITOR_0014 "Version Info List Reload DONE!"
//	#define STRMSG_S_SCMONITOR_0015 "Blocked Account List Reload DONE!"
//	#define STRMSG_S_SCMONITOR_0016 "Free server service has been stopped."
//	#define STRMSG_S_SCMONITOR_0017 "Free server service started."
//	#define STRMSG_S_SCMONITOR_0018 "Field server is not executed"
//	#define STRMSG_S_SCMONITOR_0019 "Do you really want to close Field Server?"
//	#define STRMSG_S_SCMONITOR_0020 "Update version list information(Maximum 1492 Bytes)\r\n\r\n    Number of version list[%3d], Data capacity[%4dBytes]"
//	#define STRMSG_S_SCMONITOR_0021 "Do you really want to close Pre Server?"
//	#define STRMSG_S_SCMONITOR_0022 "Do you really want to close IM Server?"
//	#define STRMSG_S_SCMONITOR_0023 "Do you really want to close NPC Server?"
//	#define STRMSG_S_SCMONITOR_0024 "%Yyear %mmonth %dday %Hhour %Mminute %Ssecond"
//	#define STRMSG_S_SCMONITOR_0025 "No event(%d)"
//	#define STRMSG_S_SCMONITOR_0026 "Open beta attendance event(%d)"
//	#define STRMSG_S_SCMONITOR_0027 "Do not know event(%d)"
//	#define STRMSG_S_SCMONITOR_0028 "Set up time for next occupying battle"
//	#define STRMSG_S_SCMONITOR_0029 "Standard time for next occupying battle"
//	#define STRMSG_S_SCMONITOR_0030 "Occupying brigade"

	// 4-2 AtumMonitor - STRERR
//	#define STRERR_S_SCMONITOR_0000 "  ==> Command succesful.\r\n"
//	#define STRERR_S_SCMONITOR_0001 "  ==> Command failed.\r\n"
//	#define STRERR_S_SCMONITOR_0002 "Cannot connect to the DB."
//	#define STRERR_S_SCMONITOR_0003 "Corresponding Version do not exist"
//	#define STRERR_S_SCMONITOR_0004 "Please enter name of the folder that you want to compress"
//	#define STRERR_S_SCMONITOR_0005 "Please enter name of the folder that you want to output"
//	#define STRERR_S_SCMONITOR_0006 "Please enter start version"
//	#define STRERR_S_SCMONITOR_0007 "Please enter last version"
//	#define STRERR_S_SCMONITOR_0008 "Please choose the folder that you are going to output Zip file for update"
//	#define STRERR_S_SCMONITOR_0009 "Cannot connect file"
//	#define STRERR_S_SCMONITOR_0010 "Please choose the server!"
//	#define STRERR_S_SCMONITOR_0011 "Cannot connect to the DB"
//	#define STRERR_S_SCMONITOR_0012 "[Error]Unable to process Message Type: %s(%#04x) in CLeftView::OnSocketNotify()\n"
//	#define STRERR_S_SCMONITOR_0013 "There is too many update version list.(Version list number[%3d], Data capacity[%4dBytes])\r\n\r\n    You must arrange version list."
//	#define STRERR_S_SCMONITOR_0014 "Eliminated function.\r\nPlease use management tool."
//	#define STRERR_S_SCMONITOR_0015 "This is not a city occupying map"
//	#define STRERR_S_SCMONITOR_0016 "Cannot make EDIT control."
//	#define STRERR_S_SCMONITOR_0017 "You have already registered existing file."

// 4_end	
///////////////////////////////////////////////////////////////////////////////
	

///////////////////////////////////////////////////////////////////////////////
// 5 - FieldServer
	// 5-1 Field<->Log
	#define STRMSG_S_F2LOGCONNECT_0000 "[Hata] WndProc(), LogServer[%15s:%4d] baglantisi kurulamadi. Yeniden baglaniliyor
"
	#define STRMSG_S_F2LOGCONNECT_0001 "Log Server'a giris yapildi.
"
	#define STRMSG_S_F2LOGCONNECT_0002 "Log Server[%15s:%4d] baglantisi kesildi. Yeniden baglaniliyor.
"

	// 5-2 Field<->Pre
	#define STRMSG_S_F2PRECONNECT_0000 "[Hata] WndProc(), PreServer[%15s:%4d] baglantisi kurulamadi. Yeniden baglaniliyor
"
	#define STRMSG_S_F2PRECONNECT_0001 "Pre Server'a giris yapildi.
"
	#define STRMSG_S_F2PRECONNECT_0002 "  %s[%s] kaynagindan T_ERROR %s(%#04X) alindi
"
	#define STRMSG_S_F2PRECONNECT_0003 "Bilinmeyen Hata@WM_PRE_PACKET_NOTIFY: %s(%#04x)
"
	#define STRMSG_S_F2PRECONNECT_0004 "Pre Server[%15s:%4d] baglantisi kesildi. Yeniden baglaniliyor.
"

	// 5-3 Field<->IM
	#define STRMSG_S_F2IMCONNECT_0000 "[Hata] WndProc(), IMServer[%15s:%4d] baglantisi kurulamadi. Yeniden baglaniliyor
"
	#define STRMSG_S_F2IMCONNECT_0001 "IM Server'a giris yapildi.
"
	#define STRMSG_S_F2IMCONNECT_0002 "IM Server[%15s:%4d] baglantisi kesildi. Yeniden baglaniliyor.
"
	#define STRMSG_S_F2IMCONNECT_0003 "  %s[%s] kaynagindan T_ERROR %s(%#04X) alindi
"
	#define STRMSG_S_F2IMCONNECT_0004 "Bilinmeyen Hata@WM_IM_PACKET_NOTIFY: %s(%#04x)
"

	// 5-3 Field - DB
	#define STRMSG_S_F2DBQUERY_0000 "Field Server sorgularinda boyle bir DB sorgusu yok! %d
"
	#define STRMSG_S_F2DBQUERY_0001 "Ilgili esya bulunmuyor."
	#define STRMSG_S_F2DBQUERY_0002 "'%s' yoklama icin basvurdu, bu nedenle vaat edilen esya"
	#define STRMSG_S_F2DBQUERY_0003 "verildi. Envanterinizi kontrol etmek icin F5'e basin"
	#define STRMSG_S_F2DBQUERY_0004 "Yetenek %s(%d) eklendi"
	#define STRMSG_S_F2DBQUERY_0005 "Ilgili esya bulunmuyor."
	#define STRMSG_S_F2DBQUERY_0006 "Esya satin alinamadi."

	// 5-4 Field - CityWar
	#define STRMSG_S_F2CITYWAR_0000 "  Sehir isgal savasi baslangici : %d(%10s) occGuildName(%s)
"
	#define STRMSG_S_F2CITYWAR_0001 "		  Katilan tugay : GuildUID(%4d) GuildName(%10s) GuildMaster(%d)
"
	#define STRMSG_S_F2CITYWAR_0002 "  Sehir isgal savasi canavar patlamasi : %d(%10s) occGuildName(%s)
"
	#define STRMSG_S_F2CITYWAR_0003 "		  Toplam hasar ==> GuildName(%10s) SumOfDamage(%8.2f)
"
	#define STRMSG_S_F2CITYWAR_0004 "  City occupying battle : %d(%10s) CityMapIndex(%d) QuestIndex(%d) OccGuildID(%d) OccGuildName(%s) OccGuildMasterUID(%d) 점령전시간(%s)\r\n"
	#define STRMSG_S_F2CITYWAR_0005 "[Hata] SetCityWarState_ DBError, MapIndex(%d)
"
	#define STRMSG_S_F2CITYWAR_0006 "%d dakika sonra \"%s\" sehir isgal savasi baslayacak."
	#define STRMSG_S_F2CITYWAR_0007 "%d dakika sonra \"%s\" sehir isgal savasi sona erecek."
	#define STRMSG_S_F2CITYWAR_0008 "Sehir isgal savasi icin canavar cagirildi(%s) : NPC isgali"
	#define STRMSG_S_F2CITYWAR_0009 "Sehir isgal savasi icin canavar cagirildi(%s) : %s tugayi isgali"
	#define STRMSG_S_F2CITYWAR_0010 "\"NPC\" su anda \"%s\" bolgesini isgal ediyor."
	#define STRMSG_S_F2CITYWAR_0011 "\"%s\" tugayi su anda \"%s\" bolgesini isgal ediyor."

	// 5-4 Field - Quest
	#define STRMSG_S_F2QUEST_0000 "Gorev yuklenemedi"
	#define STRMSG_S_F2QUEST_0001 "Gorev yuklenmedi.
"
//	#define STRMSG_S_F2QUEST_0002 "퀘스트 \'%30s\' 번호 %d -> OK\r\n"

	// 5-4 Field - config
	#define STRMSG_S_F2CONFIG_0000 "Test sunucusu ayarlandi! 

LoadFieldServerDataDebug() kaldirilmalidir! "
	#define STRMSG_S_F2NOTIFY_0000 "splash %d: %d -> %5.2f(%2.1f%%)"
	#define STRMSG_S_F2NOTIFY_0001 "canavar splash %d: %s -> %5.2f"
	#define STRMSG_S_F2NOTIFY_0002 "1. tur"
	#define STRMSG_S_F2NOTIFY_0003 "2. tur"
	#define STRMSG_S_F2NOTIFY_0004 "Mon1(%s)"
	#define STRMSG_S_F2NOTIFY_0005 "Mon2(%s)"
	#define STRMSG_S_F2NOTIFY_0006 "Yanlis silah turu! Lutfen yoneticiye bildirin!"
	#define STRMSG_S_F2NOTIFY_0007 "1-1tur: %4.1f vs %4.1f -> Saldiri olasiligi %2.2f%% azaldi "
	#define STRMSG_S_F2NOTIFY_0008 "%s->%s basarisiz, olasilik(%d>%5.2f)"
	#define STRMSG_S_F2NOTIFY_0009 "%s->%s basarisiz, olasilik(%d>%5.2f) -%5.2f"
	#define STRMSG_S_F2NOTIFY_0010 "%s->%s basarisiz, olasilik(%d>%5.2f)"
	#define STRMSG_S_F2NOTIFY_0011 "1-2tur: %4.1f vs %4.1f -> Hasar %2.2f%% azaldi(%4.1f->%4.1f)"
	#define STRMSG_S_F2NOTIFY_0012 "%s->%s, %5.2f verir(%5.2f-%d/255) (%d<=%5.2f)"
	#define STRMSG_S_F2NOTIFY_0013 "%s->%s, %5.2f alir(%5.2f-%d/255) (%d<=%5.2f)"
	#define STRMSG_S_F2NOTIFY_0014 "%s->%s(%d, HP:%5.2f), %5.2f verir(%5.2f-%d/255) (%d<=%5.2f)"
	#define STRMSG_S_F2NOTIFY_0015 "%s->%s, %5.2f alir(%5.2f-%d/255) (%d<=%5.2f)"
	#define STRMSG_S_F2NOTIFY_0016 "Sahte hedef basarisiz: Olasilik yetersiz > %d"
	#define STRMSG_S_F2NOTIFY_0017 "Kalan sahte hedef[%#08x]: %5.2f(%5.2f-%5.2f)"
	#define STRMSG_S_F2NOTIFY_0018 "Mermi bilgisi bulunmuyor. Lutfen yoneticiye bildirin."
	#define STRMSG_S_F2NOTIFY_0019 "Mermi kalibre bilgisi bulunmuyor. %s %d"
	#define STRMSG_S_F2NOTIFY_0020 "Bu mermi bilgisidir. %s %d"
	#define STRMSG_S_F2NOTIFY_0021 "Etkinlik devam ederken warp yapilamaz"
	#define STRMSG_S_F2NOTIFY_0022 "Oluyken warp yapilamaz."
	#define STRMSG_S_F2NOTIFY_0023 "Formasyon savasindayken warp yapilamaz"
	#define STRMSG_S_F2NOTIFY_0024 "Harita uretim hatasi! Lutfen yoneticiye bildirin!"
	#define STRMSG_S_F2NOTIFY_0025 "Harita uretim hatasi! Lutfen yoneticiye bildirin! %d, %d numarali warp hedef indeksi bulunmuyor!
"
	#define STRMSG_S_F2NOTIFY_0026 "Formasyon savasi devam ederken warp yapilamaz"
	#define STRMSG_S_F2NOTIFY_0027 "  Process_FC_CHARACTER_DEAD_GAMESTART() icinde WARP(%04d) islenemiyor, %s
"
	#define STRMSG_S_F2NOTIFY_0028 "  1 -> Karakter %10s, %5.2f hasar alir"
	#define STRMSG_S_F2NOTIFY_0029 "1 -> Karakter %10s, %5.2f hasar verir"
	#define STRMSG_S_F2NOTIFY_0030 "1 -> Karakter %10s, %5.2f hasar verir"
	#define STRMSG_S_F2NOTIFY_0031 "1 -> Karakter %10s, %5.2f hasar verir"
	#define STRMSG_S_F2NOTIFY_0032 "  2 -> Karakter %10s, %5.2f hasar alir"
	#define STRMSG_S_F2NOTIFY_0033 "2 -> Karakter %10s, %5.2f hasar verir"
	#define STRMSG_S_F2NOTIFY_0034 "2 -> Canavar %3d, %5.2f hasar verir(%d)"
	#define STRMSG_S_F2NOTIFY_0035 "2 -> Karakter %10s, %5.2f hasar verir"
	#define STRMSG_S_F2NOTIFY_0036 "2 -> Karakter %10s, %5.2f sahte hedef hasari verir"
	#define STRMSG_S_F2NOTIFY_0037 "  2 -> Canavar %3d, %5.2f hasar alir"
	#define STRMSG_S_F2NOTIFY_0038 "  MAYIN -> Karakter %10s, %5.2f hasar verir"
	#define STRMSG_S_F2NOTIFY_0039 "  MAYIN -> Karakter %10s, %5.2f sahte hedef hasari verir"
	#define STRMSG_S_F2NOTIFY_0040 "  MAYIN -> Canavar %10s, %5.2f hasar verir"
	#define STRMSG_S_F2NOTIFY_0041 "Stat sifirlama basarili."
	#define STRMSG_S_F2NOTIFY_0042 "Oluyken bunu kullanamazsiniz"
	#define STRMSG_S_F2NOTIFY_0043 "ENCHANT_INFO bulunmuyor"
	#define STRMSG_S_F2NOTIFY_0044 "Enchant basarili."
	#define STRMSG_S_F2NOTIFY_0045 "Enchant basarisiz."
	#define STRMSG_S_F2NOTIFY_0046 "Tugaya ait degil."
	#define STRMSG_S_F2NOTIFY_0047 "Istek icin zaten bekleme durumunda."
	#define STRMSG_S_F2NOTIFY_0048 "Lutfen daha sonra tekrar deneyin."
	#define STRMSG_S_F2NOTIFY_0049 "Siz veya diger kisi tugay lideri."
	#define STRMSG_S_F2NOTIFY_0050 "Zaten tugay savasinda."
	#define STRMSG_S_F2NOTIFY_0051 "Iki tugay lideri ayni haritada degil."
	#define STRMSG_S_F2NOTIFY_0052 "Tum formasyon uyeleri ayni haritada degil"
	#define STRMSG_S_F2NOTIFY_0053 "Tum formasyon uyeleri ayni haritada degil"
	#define STRMSG_S_F2NOTIFY_0054 "Tugaya ait degilsiniz."
	#define STRMSG_S_F2NOTIFY_0055 "Istek alicisi farkli."
	#define STRMSG_S_F2NOTIFY_0056 "Siz veya diger kisi tugay lideri degil."
	#define STRMSG_S_F2NOTIFY_0057 "Iki tugay lideri ayni haritada degil."
	#define STRMSG_S_F2NOTIFY_0058 "Tugaya ait degilsiniz."
	#define STRMSG_S_F2NOTIFY_0059 "Istek alicisi farkli."
	#define STRMSG_S_F2NOTIFY_0060 "Siz veya diger kisi tugay lideri degil."
	#define STRMSG_S_F2NOTIFY_0061 "Sehir isgal savasi savunmasi basarili"
	#define STRMSG_S_F2NOTIFY_0062 "Gorev ayar hatasi. Lutfen yoneticiye bildirin."
	#define STRMSG_S_F2NOTIFY_0063 "Ilgili esya(%s) bu konuma takilamaz"
	#define STRMSG_S_F2NOTIFY_0064 "Motor yuvasi bos birakilamaz."
	#define STRMSG_S_F2NOTIFY_0065 "Esya tasima: (%I64d, %d) -> (%I64d, %d)"
	#define STRMSG_S_F2NOTIFY_0066 "Hatali esya tasima: (%I64d, %d) -> (%I64d, %d)"
	#define STRMSG_S_F2NOTIFY_0067 "  Process_FC_EVENT_REQUEST_WARP() icinde EVENT(%d) islenemiyor, %s
"
	#define STRMSG_S_F2NOTIFY_0068 "Harita etkinlik bilgisi anormal!!! Lutfen yoneticiye bildirin!!!"
	#define STRMSG_S_F2NOTIFY_0069 "Ilgili warp hedefi bulunmuyor"
	#define STRMSG_S_F2NOTIFY_0070 "Esya satin alma hatasi. Lutfen yoneticiye bildirin."
	#define STRMSG_S_F2NOTIFY_0071 "Satin almak istediginiz yetenek seviyesi mevcut seviyenizden dusuk veya esit."
	#define STRMSG_S_F2NOTIFY_0072 "Esya hatasi. Esya bir yetenek satin alimi gerektiriyor."
	#define STRMSG_S_F2NOTIFY_0073 "Esya satis hatasi. Lutfen yoneticiye bildirin."
	#define STRMSG_S_F2NOTIFY_0074 "Esya satin alma hatasi. Lutfen yoneticiye bildirin."
	#define STRMSG_S_F2NOTIFY_0075 "'Kredi esyasi' satin alindi."
	#define STRMSG_S_F2NOTIFY_0076 "  satin alma listesi : '%s(%dadet)'"
	#define STRMSG_S_F2NOTIFY_0077 "Formasyona zaten katildiniz."
	#define STRMSG_S_F2NOTIFY_0078 "Formasyon lideriyseniz katilamazsiniz"
	#define STRMSG_S_F2NOTIFY_0079 "Formasyon savasi devam ederken katilamazsiniz."
	#define STRMSG_S_F2NOTIFY_0080 "Tum formasyon personel bilgisini getirme desteklenmiyor!"
	#define STRMSG_S_F2NOTIFY_0081 "Harita etkinlik bilgisi anormal!!! Lutfen yoneticiye bildirin!!!"
	#define STRMSG_S_F2NOTIFY_0082 "Bu hesap ticaret yapamaz"
	#define STRMSG_S_F2NOTIFY_0083 "%s kapasitesini asti."
	#define STRMSG_S_F2NOTIFY_0084 "Olası hareket koordinati: (5, 5) -> (%d, %d)"
	#define STRMSG_S_F2NOTIFY_0085 "Kullanici: "
	#define STRMSG_S_F2NOTIFY_0086 "En fazla 20 kisi goruntulenebilir."
	#define STRMSG_S_F2NOTIFY_0087 "Ilgili kullanici(%s) bulunmuyor"
	#define STRMSG_S_F2NOTIFY_0088 "Formasyona ait degil"
	#define STRMSG_S_F2NOTIFY_0089 "Mevcut zaman: %d:%d, Atum zamani: %d:%d"
	#define STRMSG_S_F2NOTIFY_0090 "Degisen zaman: %d:%d, Atum zamani: %d:%d"
	#define STRMSG_S_F2NOTIFY_0091 "NPC sunucusuna bagli degil"
	#define STRMSG_S_F2NOTIFY_0092 "Ilgili esya(%d) bulunmuyor"
	#define STRMSG_S_F2NOTIFY_0093 "Yiginlanabilir bir esya degilse en fazla 10 adet"
	#define STRMSG_S_F2NOTIFY_0094 "Harita '%s' toplam bagli kisi sayisi: %dkisi"
	#define STRMSG_S_F2NOTIFY_0095 "Harita %s %s es zamanli baglanti: %dkisi - '%s'(*)"
	#define STRMSG_S_F2NOTIFY_0096 "Harita %s %s es zamanli baglanti: %dkisi - '%s'"
	#define STRMSG_S_F2NOTIFY_0097 "Mevcut harita kanali: %s, %d(%d)"
	#define STRMSG_S_F2NOTIFY_0098 "Mermi bilgisi bulunmuyor. Lutfen yoneticiye bildirin."
	#define STRMSG_S_F2NOTIFY_0099 "Mermi kalibre bilgisi bulunmuyor. %s %d"
	#define STRMSG_S_F2NOTIFY_0100 "Mermi bilgisi bulunmuyor. Lutfen yoneticiye bildirin."
	#define STRMSG_S_F2NOTIFY_0101 "Mermi kalibre bilgisi bulunmuyor. %s %d"
	#define STRMSG_S_F2NOTIFY_0102 "Ilgili kullanici(%s) bulunmuyor"
	#define STRMSG_S_F2NOTIFY_0103 "Corresponding user(%s)is dead"
	#define STRMSG_S_F2NOTIFY_0104 "Corresponding users(%s)do not exist"
	#define STRMSG_S_F2NOTIFY_0105 "Set as a Standard Account"
	#define STRMSG_S_F2NOTIFY_0106 "Standard Account has been cancelled"
	#define STRMSG_S_F2NOTIFY_0107 "Invincibility has been turned on."
	#define STRMSG_S_F2NOTIFY_0108 "Invincibility has been turned off."
	#define STRMSG_S_F2NOTIFY_0109 "Weapon damage will be modified by %5.0f%% "
	#define STRMSG_S_F2NOTIFY_0110 "Will be initialized when reloading the item"
	#define STRMSG_S_F2NOTIFY_0111 "Will be initialized when reloading the item"
	#define STRMSG_S_F2NOTIFY_0112 "Corresponding user(%s)does not exist"
	#define STRMSG_S_F2NOTIFY_0113 "Invisibility has been deactivated."
	#define STRMSG_S_F2NOTIFY_0114 "Invisibility has been activated."
	#define STRMSG_S_F2NOTIFY_0115 "%s event is not under progress"
	#define STRMSG_S_F2NOTIFY_0116 "%s event has started(magnification:%4.2f, Event time:%3dminutes)"
	#define STRMSG_S_F2NOTIFY_0117 "Set as standard premium Account"
	#define STRMSG_S_F2NOTIFY_0118 "Failed to set as a premium Account"
	#define STRMSG_S_F2NOTIFY_0119 "Set as a super premium Account"
	#define STRMSG_S_F2NOTIFY_0120 "This is not the map for city occupying battle"
	#define STRMSG_S_F2NOTIFY_0121 "City occupying battle cannot be started"
	#define STRMSG_S_F2NOTIFY_0122 "City occupying battle cannot be finished"
	#define STRMSG_S_F2NOTIFY_0123 "Stealth mode has been initialized"
	#define STRMSG_S_F2NOTIFY_0124 "In stealth state"
	#define STRMSG_S_F2NOTIFY_0125 "Map is not validated."
	#define STRMSG_S_F2NOTIFY_0126 "Happy hour event set [progress time:%4dminute)]"
	#define STRMSG_S_F2NOTIFY_0127 "Happy hour event finished"
	#define STRMSG_S_F2NOTIFY_0128 "  1 -> From monster %3d ,received damage %5.2f"
	#define STRMSG_S_F2NOTIFY_0129 "  1type -> From monster %3d ,received damage %5.2f(dummy)"
	#define STRMSG_S_F2NOTIFY_0130 "Logged in with %s authority"
	#define STRMSG_S_F2NOTIFY_0131 "  Do not process completing routine %s: CS(%d), DBStore(%d)\r\n"
	#define STRMSG_S_F2NOTIFY_0132 "  ERROR@CharacterGameEndRoutine(): Failed to eliminate party personnel! %s\r\n"
// 2005-11-24 by cmkwon, 
//	#define STRMSG_S_F2NOTIFY_0133 "Prefix \'%s\' 성공: %d <= %d <= %d\r\n"
//	#define STRMSG_S_F2NOTIFY_0134 "Suffix \'%s\' 성공: %d <= %d <= %d\r\n"
	#define STRMSG_S_F2NOTIFY_0135 "%s: Stopped!"
	#define STRMSG_S_F2NOTIFY_0136 "FATAL ERROR: Please make an inquiry to the administrator! Shop variable allotment error!"
	#define STRMSG_S_F2NOTIFY_0137 "%s is deleted."
	#define STRMSG_S_F2NOTIFY_0138 "The channel that you are presently at is deactivated so monsters and other functions are not available."
	#define STRMSG_S_F2NOTIFY_0139 "Stopped. Please use another channel."
	#define STRMSG_S_F2NOTIFY_0140 "  Warp Obj.[%s,%2dpieces]: %04d[%1s%4d]"
	#define STRMSG_S_F2NOTIFY_0141 "Timer movement %s\r\n"
	#define STRMSG_S_F2NOTIFY_0142 "HP auto recovery stopped"
	#define STRMSG_S_F2NOTIFY_0143 "HP auto recovery has stopped as this is not a possible location for landing"
	#define STRMSG_S_F2NOTIFY_0144 "HP 5.2f recovery for emergency"
	#define STRMSG_S_F2NOTIFY_0145 "Gradual HP UP Stopped(remaining time: %d)"
	#define STRMSG_S_F2NOTIFY_0146 "Gradual DP UP Stopped(remaining time: %d)"
	#define STRMSG_S_F2NOTIFY_0147 "Gradual EP UP Stopped(remaining time: %d)"
	#define STRMSG_S_F2NOTIFY_0148 "%s event has been completed."
	#define STRMSG_S_F2NOTIFY_0149 "%s event under progress (magnification %4.2f, remaining:%3dminute)"
	#define STRMSG_S_F2NOTIFY_0150 "Command has not been exercised"


	// 5-5 Field - NOTIFY Error
	#define STRERR_S_F2NOTIFY_0000 "	Experience point division error(%s, %s(%d)): fTotalDamage(%d) < 0.0f or Empty Vector: %d, Distance(%5.1f)\n"
	#define STRERR_S_F2NOTIFY_0001 "  Not valid Game Start message error ClientState[%d]\r\n"
	#define STRERR_S_F2NOTIFY_0002 "  Not valid Game Start message error ClientState[%d]\r\n"
	#define STRERR_S_F2NOTIFY_0003 "  WARP(%04d) treatment impossible at ProcessQuestResult(), %s\r\n"
	#define STRERR_S_F2NOTIFY_0004 "Map event information abnormal!!! Please make inquiry to the administrator!!! Current(%s, %s, %04d), Target(%04d, %d)\r\n"
	#define STRERR_S_F2NOTIFY_0005 "  EVENT(%d) treatment impossible at T_FC_PARTY_REQUEST_PARTY_WARP(), %s\r\n"
	#define STRERR_S_F2NOTIFY_0006 "Map event information abnormal!!! Please make inquiry to the administrator!!! Current(%s, %s, %04d), Target(%04d, %d)\r\n"
	#define STRERR_S_F2NOTIFY_0007 "  WARP treatment impossible at T_FC_PARTY_REQUEST_PARTY_WARP(). %s\r\n"
	#define STRERR_S_F2NOTIFY_0008 "  EVENT(%d) treatment impossible(party leader) at Process_FC_PARTY_REQUEST_PARTY_WARP_WITH_MAP_NAME(). %s\r\n"
	#define STRERR_S_F2NOTIFY_0009 "  WARP(%04d) treatment impossible at Process_FC_PARTY_REQUEST_PARTY_WARP_WITH_MAP_NAME(). %s\r\n"
	#define STRERR_S_F2NOTIFY_0010 "  EVENT(%d) treatment impossible at Process_FC_EVENT_REQUEST_WARP(), %s\r\n"
	#define STRERR_S_F2NOTIFY_0011 "  WARP treatment impossible at T_FC_PARTY_REQUEST_PARTY_WARP(). %s\r\n"
	#define STRERR_S_F2NOTIFY_0012 "Party warp failed: %s -> distance: %5.2f, bodycon: %d, %d"
	#define STRERR_S_F2NOTIFY_0013 "  WARP(%04d) treatment impossible at Process_FC_CHARACTER_DEAD_GAMESTART(), %s\r\n"
	#define STRERR_S_F2NOTIFY_0014 "  %s -> click overlapped OK button!!!\r\n"
	#define STRERR_S_F2NOTIFY_0015 "Quest has not been loaded.\r\n"
	#define STRERR_S_F2NOTIFY_0016 "  WARP(%s) treatment impossible at HandleAdminCommands(), /move, %s\r\n"
	#define STRERR_S_F2NOTIFY_0017 "  WARP(%s) treatment impossible at HandleAdminCommands(), /send, %s\r\n"
	#define STRERR_S_F2NOTIFY_0018 "  FATAL ERROR: This message should not be sent to field server that members do not belong to. check it out\r\n"
	#define STRERR_S_F2NOTIFY_0019 "  WARP(%04d) treatment impossible at T_FI_ADMIN_CALL_CHARACTER, %s\r\n"
	#define STRERR_S_F2NOTIFY_0020 "  WARP(%s) treatment impossible at T_FI_ADMIN_MOVETO_CHARACTER, %s\r\n"

	// 5-6 Field - Event
	#define STRMSG_S_F2EVENTTYPE_0000 "Experience point"
	#define STRMSG_S_F2EVENTTYPE_0001 "SPI"
	#define STRMSG_S_F2EVENTTYPE_0002 "Restoration of experience point"
	#define STRMSG_S_F2EVENTTYPE_0003 "Item drop"
	#define STRMSG_S_F2EVENTTYPE_0004 "Rare item drop"
	#define STRMSG_S_F2EVENTTYPE_0005 "War Point event"
// 5_end	
///////////////////////////////////////////////////////////////////////////////
	

///////////////////////////////////////////////////////////////////////////////
// 6 - IMServer
	// 6-1 IM<->Pre
	#define STRMSG_S_I2PRECONNECT_0000 "Logged into Pre Server.\n"
	#define STRMSG_S_I2PRECONNECT_0001 "Connection to Pre Server[%15s:%4d]has been lost. Attempting reconnection.\r\n"

	// 6-2 IM Notify
	#define STRMSG_S_I2NOTIFY_0000 "\'%s\' already exists"
	#define STRMSG_S_I2NOTIFY_0001 "\'%s\' is already in a brigade."
	#define STRMSG_S_I2NOTIFY_0002 "\'%s\' is already the name of an existing brigade"
	#define STRMSG_S_I2NOTIFY_0003 "Brigade loading failed - Please make an inquiry to the administrator!"
	#define STRMSG_S_I2NOTIFY_0004 "Your friend '%s' has logged in"
	#define STRMSG_S_I2NOTIFY_0005 "Checking previous formation"
	#define STRMSG_S_I2NOTIFY_0006 "Previous formation does not exist"
	#define STRMSG_S_I2NOTIFY_0007 "Rejoined the formation"
	#define STRMSG_S_I2NOTIFY_0008 "You are currently in a formation"
	#define STRMSG_S_I2NOTIFY_0009 "  Fatal Error: Fatal party error! at T_IC_PARTY_GET_MEMBER"
	#define STRMSG_S_I2NOTIFY_0010 "State of formation personnel is invalid"
	#define STRMSG_S_I2NOTIFY_0011 "formation personnel have already joined the brigade"
	#define STRMSG_S_I2NOTIFY_0012 "That person has already joined the brigade"
	#define STRMSG_S_I2NOTIFY_0013 "You cannot invite yourself"
	#define STRMSG_S_I2NOTIFY_0014 "You are in a state where joining brigades is not allowed"
	#define STRMSG_S_I2NOTIFY_0015 "You have exceeded the number of brigade personnel that can join"
	#define STRMSG_S_I2NOTIFY_0016 "You are in a state where joining brigades is not allowed"
	#define STRMSG_S_I2NOTIFY_0017 "Brigade leader cannot withdraw from brigade"
	#define STRMSG_S_I2NOTIFY_0018 "Cannot withdraw from brigade while you are in a brigade fight"
	#define STRMSG_S_I2NOTIFY_0019 "Cannot expel brigade personnel while in brigade fight"
	#define STRMSG_S_I2NOTIFY_0020 "Brigade leader cannot be expelled"
	#define STRMSG_S_I2NOTIFY_0021 "Cannot disperse brigade while you are in a brigade fight"
	#define STRMSG_S_I2NOTIFY_0022 "Cannot change to the same name"
	#define STRMSG_S_I2NOTIFY_0023 "You are in a state where the brigade name cannot be changed"
	#define STRMSG_S_I2NOTIFY_0024 "You are in a state where the brigade mark cannot be changed"
	#define STRMSG_S_I2NOTIFY_0025 "You are in a state where rank cannot be changed"
	#define STRMSG_S_I2NOTIFY_0026 "Rank has overlapped"
	#define STRMSG_S_I2NOTIFY_0027 "Rank change to regiment leader is not possible."
	#define STRMSG_S_I2NOTIFY_0028 "Change in regiment leaders rank is not possible."
	#define STRMSG_S_I2NOTIFY_0029 "Number of whole users: %dpersons (output up to %dpeople)"
	#define STRMSG_S_I2NOTIFY_0030 "Added to administrators message list"
	#define STRMSG_S_I2NOTIFY_0031 "Deleted from administrators message list"
	#define STRMSG_S_I2NOTIFY_0032 "Server IP: %s"
	#define STRMSG_S_I2NOTIFY_0033 "Server group \'%s\' total number of people logged on: %dpeople"
	#define STRMSG_S_I2NOTIFY_0034 "Do you really want to close the server(%s)? number: %d"
	#define STRMSG_S_I2NOTIFY_0035 "\'%s\'is not playing the game "
	#define STRMSG_S_I2NOTIFY_0036 "Number of whole users: %dpersons (Output up to %dpeople)"
	#define STRMSG_S_I2NOTIFY_0037 "/send %s %s"
	#define STRMSG_S_I2NOTIFY_0038 "Corresponding regiment does not exist."
	#define STRMSG_S_I2NOTIFY_0039 "Whisper block has been disabled"
	#define STRMSG_S_I2NOTIFY_0040 "Whisper has been blocked"
	#define STRMSG_S_I2NOTIFY_0041 "Did not join the brigade"
	#define STRMSG_S_I2NOTIFY_0042 "Brigade(%d) is not available"
	#define STRMSG_S_I2NOTIFY_0043 "Corresponding weather(%s)does not exist"
	#define STRMSG_S_I2NOTIFY_0044 "Field Server is invalid"
	#define STRMSG_S_I2NOTIFY_0045 "Corresponding map(%s) does not exist"
	#define STRMSG_S_I2NOTIFY_0046 "You cannot chat for %d!minutes!!"
	#define STRMSG_S_I2NOTIFY_0047 "Chatting block set : '%10s', %3dminute"
	#define STRMSG_S_I2NOTIFY_0048 "Chatting block condition has been cancelled."
	#define STRMSG_S_I2NOTIFY_0049 "Chatting block cancelled: '%10s'"
	#define STRMSG_S_I2NOTIFY_0050 "%s(account: %s, map: %d(%d), level: %d) PLAYING"
	#define STRMSG_S_I2NOTIFY_0051 "Cannot summon yourself."
	#define STRMSG_S_I2NOTIFY_0052 "Corresponding brigade does not exist."

	#define STRMSG_S_IMSERVER_050607_0001	"Corresponding map does not exist."
// 6_end	
///////////////////////////////////////////////////////////////////////////////
	

///////////////////////////////////////////////////////////////////////////////
// 7 - NPCServer
	// 7-1 NPC<->Field
	#define STRMSG_S_N2FIELDCONNECT_0000 "Logged into field Server.\n"
	#define STRMSG_S_N2FIELDCONNECT_0001 "Connection to field Server[%15s:%4d]has been lost. Attempting reconnection.\r\n"

	// 7-2 IM Notify
	#define STRMSG_S_N2NOTIFY_0000 "							Collision check between monster and object does not exist\r\n"
	#define STRMSG_S_N2NOTIFY_0001 "City occupying battle monster(%10s) summoned\r\n\r\n"

	#define STRMSG_S_N2TESTMONNAME_0000 "Chul min ho"
// 7_end	
///////////////////////////////////////////////////////////////////////////////
	

///////////////////////////////////////////////////////////////////////////////
// 8 - PreServer
	// 8-1 Pre Notify
	#define STRMSG_S_P2PRENOTIFY_0000 "Logged in with %s account(%s). IP: %s\r\n"
	#define STRMSG_S_P2PRENOTIFY_0001 "Success"
	#define STRMSG_S_P2PRENOTIFY_0002 "Failure"
	#define STRMSG_S_P2PRENOTIFY_0003 "[ERROR] Error in adding account information, AccountName(%s)  privateIP(%15s)\n"


	#define STRMSG_SCAT_051115_0001		"Hello. This is the ACE Online Administrator."
	#define STRMSG_SCAT_051115_0002		"We will be performing our regular maintenance in 10 minutes."
	#define STRMSG_SCAT_051115_0003		"Please move to a safe area and exit from the game."
	#define STRMSG_SCAT_051115_0004		"First, NPC server will be shut down."
	#define STRMSG_SCAT_051115_0005		"ACE Online regular maintenance will begin."
	#define STRMSG_SCAT_051115_0006		"Please enjoy your day with ACE Online."
	#define STRMSG_SCAT_051115_0007		"There will be a server checkup in 5 minutes." // 5분 후 서버 점검이 있을 예정입니다.
	#define STRMSG_SCAT_051115_0008		"Server will be down for 60 minutes."	// 서버는 60분 동안 내려질 예정입니다


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

	// 2006-05-09 by cmkwon
	#define STRMSG_060509_0000			"Invisible Mode: Users cannot see you but weapons cannot be used."
	#define STRMSG_060509_0001			"Invincible Status: You will not receive any damage."
	#define STRMSG_060509_0002			"Stealth Mode: Monsters will not attack you first."

	// 2006-05-26 by cmkwon
	#define	STRMSG_060526_0000			"Your request has been sent to a GM. One of our GMs will contact you as soon as possible."
	#define	STRMSG_060526_0001			"You were unable to automatically update.\r\n\r\nPlease download the patch from Homepage(%s) and connect again.\r\n\r\n    Error: %s"

	// 2006-08-24 by cmkwon
	#define STRMSG_060824_0000			"This ID has not been registered or the ID and password do not match."
	
	// 2006-09-27 by cmkwon
	#define STRMSG_060927_0000			"The game server is currently offline for maintenance. Please check the official homepage (https://oldschoolrivals.com) for further information."

	// 2006-10-11 by cmkwon
	#define STRERR_061011_0000			"Game client version is not valid.\r\n  Please reinstall or download the patch file."

	// 2006-11-07 by cmkwon
	#define STRMSG_061107_0000			"You were killed by %s."

	// 2006-11-07 by cmkwon
	#define STRMSG_070410_0000   	"Jamboree server DB(atum2_db_20) initialization must be conducted after closing the jamboree server."
	#define STRMSG_070410_0001   	"Do you really want to initialize jamboree server DB(atum2_db_20)- [Validation number:%d]"
	#define STRMSG_070410_0002   	"Jamboree server  DB(atum2_db_20) is initialized."
	#define STRMSG_070410_0003   	"Jamboree server  DB(atum2_db_20) initialization failed !!"
	#define STRMSG_070410_0004   	"'%s' data duplication to Jamboree server  DB(atum2_db_20) completed."
	#define STRMSG_070410_0005   	"'%s' data duplication to Jamboree server  DB(atum2_db_20) failed - character do not exist!!"
	#define STRMSG_070410_0006   	" %s' data duplication to Jamboree server  DB(atum2_db_20) failed - Corresponding account character exists !!"
	#define STRMSG_070410_0007   	" %s' data duplication to Jamboree server  DB(atum2_db_20) failed - DB addition failure !!"
	#define STRMSG_070410_0008   	" %s' data duplication to Jamboree server  DB(atum2_db_20) failed - Unknown (%d)!!"

///////////////////////////////////////////////////////////////////////////////
// 2007-05-07 by cmkwon, 해상도 문자열 
	// 2007-07-24 by cmkwon, 런처에서 800*600 해상도 삭제 - 콤보박스 스트링 필요 없음
	//#define STRMSG_WINDOW_DEGREE_800x600_LOW			"800x600 (low)"
	//#define STRMSG_WINDOW_DEGREE_800x600_MEDIUM			"800x600 (medium)"
	//#define STRMSG_WINDOW_DEGREE_800x600_HIGH			"800x600 (high)"
#define STRMSG_WINDOW_DEGREE_1024x768_LOW			"1024x768 L"
#define STRMSG_WINDOW_DEGREE_1024x768_MEDIUM		"1024x768 M"
#define STRMSG_WINDOW_DEGREE_1024x768_HIGH			"1024x768 H"
#define STRMSG_WINDOW_DEGREE_W1280x800_LOW			"1280x800 L"
#define STRMSG_WINDOW_DEGREE_W1280x800_MEDIUM		"1280x800 M"
#define STRMSG_WINDOW_DEGREE_W1280x800_HIGH			"1280x800 H"
#define STRMSG_WINDOW_DEGREE_1280x960_LOW			"1280x960 L"
#define STRMSG_WINDOW_DEGREE_1280x960_MEDIUM		"1280x960 M"
#define STRMSG_WINDOW_DEGREE_1280x960_HIGH			"1280x960 H"
#define STRMSG_WINDOW_DEGREE_1280x1024_LOW			"1280x1024 L"
#define STRMSG_WINDOW_DEGREE_1280x1024_MEDIUM		"1280x1024 M"
#define STRMSG_WINDOW_DEGREE_1280x1024_HIGH			"1280x1024 H"
#define STRMSG_WINDOW_DEGREE_W1600x900_LOW			"1600x900 L"
#define STRMSG_WINDOW_DEGREE_W1600x900_MEDIUM		"1600x900 M"
#define STRMSG_WINDOW_DEGREE_W1600x900_HIGH			"1600x900 H"
#define STRMSG_WINDOW_DEGREE_1600x1200_LOW			"1600x1200 L"
#define STRMSG_WINDOW_DEGREE_1600x1200_MEDIUM		"1600x1200 M"
#define STRMSG_WINDOW_DEGREE_1600x1200_HIGH			"1600x1200 H"

// 2007-06-15 by dhjin, 관전 관련 스트링
#define STRMSG_070615_0000		"Cannot start as you are not in Stealth Mode."
#define STRMSG_070615_0001		"Cannot start the watch."
#define STRMSG_070620_0000	"User cannot start the watch."

// 2007-06-26 by dhjin, 워포인트 이벤트 관련 추가
#define STRMSG_S_F2EVENTTYPE_0006		"War Point"

// 2007-06-28 by cmkwon, 중국 방심취관련(게임 시간 알림 구현) - 스트링 추가
#define STRMSG_070628_0000				"You have been playing the game for %d hour(s)."
#define STRMSG_070628_0001				"You have been playing the game for %d hour(s). Please take necessary steps to get some rest."
#define STRMSG_070628_0002				"You have played too long. It is becoming dangerous for your health. Please take care of your own health and close the game. This is for your own benefit. "
#define STRMSG_070628_0003				"Playing this game any more will be critical to your health. Look after your own health by closing the game right now. If not, your health will be influenced, and your game revenue will reach 0%. Your game revenue will be back to normal 5 hours after game has been closed."

///////////////////////////////////////////////////////////////////////////////
// 2007-07-11 by cmkwon, Arena block system materialization - added string
#define STRMSG_070711_0000    "\'%s\' is not prohibited in entering arena."
#define STRMSG_070711_0001 "\'%s\' is prohibited in entering arena (time remaining: %d minutes)"
#define STRMSG_070711_0002 "You are forbidden from entering arena for %d minutes!!"
#define STRMSG_070711_0003 "Your prohibition from entering arena is discharged."
#define STRMSG_070711_0004 "Use of Arena is prohibited for \'%s\' user."

///////////////////////////////////////////////////////////////////////////////
// 2007-08-23 by cmkwon, Wide 해상도 1280x720(16:9) 추가 - 스트링 추가
#define STRMSG_WINDOW_DEGREE_W1280x720_LOW			"1280x720 (low-wide)"
#define STRMSG_WINDOW_DEGREE_W1280x720_MEDIUM		"1280x720 (medium-wide)"
#define STRMSG_WINDOW_DEGREE_W1280x720_HIGH			"1280x720 (high-wide)"

// 2007-08-30 by cmkwon, 회의룸 시스템 구현 - 스트링 추가
#define STRMSG_070830_0001                                   "This command can only be used after nation has been selected."
#define STRMSG_070830_0002                                   "Conference room map(%d) of corresponding nation is not available"
#define STRMSG_070830_0003                                   "Number of users that can enter conference room : %dpeople(s)"
#define STRMSG_070830_0004                                   "'%s' has already been added to entrance list."
#define STRMSG_070830_0005                                   "'%s' does not have entrance permission."
#define STRMSG_070830_0006                                   "'%s' is not a valid character."
#define STRMSG_070830_0007                                 "'%s' cannot be added to conference room entrance list.(Maximum %d people)"
#define STRMSG_070830_0008                                   "'%s' have received the authority of entering conference room."
#define STRMSG_070830_0009                                   "Permission to enter conference room has been given."
#define STRMSG_070830_0010                                   "Permission to enter conference room has been cancelled."
#define STRMSG_070830_0011                                   "'%s'"

// 2007-11-13 by cmkwon, just for Korean service, 
#define STRMSG_071115_0001									"\\y%s send you %s as a present."
#define STRMSG_071115_0002									"Present list : \'%s(%d pieces)\'"
#define STRMSG_071115_0003									"\\y%s send a present to %s.  Present is  %s. Please check the warehouse."
 
// 2007-11-19 by cmkwon, callGM system 
#define STRMSG_071120_0001									"Appeal system is not in effect. Please use the customer centre."
#define STRMSG_071120_0002									"Appeal system is activated."
#define STRMSG_071120_0003									"Appeal system is concluded."
#define STRMSG_071120_0004									"Appeal system will start from %s to %s."

// 2007-11-28 by cmkwon, 통지시스템 구현 - 
#define STRMSG_071128_0001									"%s has sent you %s(%d) as a present. Please check at your warehouse."

// 2007-12-27 by cmkwon, 윈도우즈 모드 기능 추가 - 
#define STRMSG_071228_0001				"Resolution setting is invalid. Please check again."

// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define STRMSG_080201_0001									"'Error exists in %s' block setting. ErrorCode(%d)"
#define STRMSG_080201_0002									"'%s' account has been blocked.[Block End Date: %s]"
#define STRMSG_080201_0003									"'%s' account does not exist in block list. ErrorCode(%d)"
#define STRMSG_080201_0004									"'Erros has occured in the process of blocking %s' account. ErrorCode(%d)"
#define STRMSG_080201_0005									"'%s' account has been removed from block list." 

// 2008-02-11 by cmkwon, 해상도 추가(1440x900) - 
#define STRMSG_WINDOW_DEGREE_1440x900_LOW			"1440x900 (low-wide)"
#define STRMSG_WINDOW_DEGREE_1440x900_MEDIUM		"1440x900 (medium-wide)"
#define STRMSG_WINDOW_DEGREE_1440x900_HIGH			"1440x900 (high-wide)"

// 2007-12-27 by dhjin, 아레나통합- 아레나서버연결관련오류
#define STRMSG_S_MF2AFCONNECT_0000                       "[Error] WndProc(), Can't connect to  ArenaServer[%15s:%4d] Reconnect\r\n"
#define STRMSG_S_MF2AFCONNECT_0001                       "Connected to Arena Server.\r\n"
#define STRMSG_S_MF2AFCONNECT_0002                       "Connection to Arena Server[%15s:%4d] is closed. Trying to re-connect.\r\n"
#define STRMSG_S_MF2AFCONNECT_0003                       "  T_ERROR %s(%#04X) received from %s[%s]\r\n"
#define STRMSG_S_MF2AFCONNECT_0004                       "Unknown Error@WM_FIELD_PACKET_NOTIFY: %s(%#04x)\n"
#define STRMSG_ARENAEVENT_080310_0001                    "Number \\y%d Arena waiting room has been given event attribute.\r\n"
#define STRMSG_ARENAEVENT_080310_0002                    "Number \\y%d Arena waiting room has been discharged from event attribute.\r\n"
#define STRMSG_ARENAEVENT_080310_0003                    "\\yFailed to give Arena attribute.\r\n"
#define STRMSG_080428_0001					"\\y%s has been shot down.\\y"          // 2008-04-28 by dhjin, Arena integration - String is added when taking down the opponent, only in Arena map 

// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - 
#define STRMSG_080430_0001					"Selected server is a server where creation of new character is not available."

// 2008-06-13 by dhjin, EP3 여단 수정 사항 - 
#define STRMSG_080613_0001					"Request for entering %s brigade was rejected."

// 2008-09-04 by cmkwon, don't need translation, SystemLog 
#define STRMSG_080904_0001					 "[DB Error] No such DB query process(QP_xxx) function !! QueryType(%d:%s)\r\n"


// 2008-12-30 by cmkwon, 지도자 채팅 제한 카드 구현 - 
#define STRMSG_081230_0001					"\\y%s will be restricted from chatting for %d minutes.\\y"
#define STRMSG_081230_0002					"\\yChatting has been restricted by the leader for %d minutes.\\y"
#define STRMSG_081230_0003					"\\yChatting restriction by leader is released.\\y"

///////////////////////////////////////////////////////////////////////////////
// 2009-08-31 by cmkwon, Gameforge4D 게임가드 동의창 띄우기 - 
// 2009-09-02 by cmkwon, Gameforge4D 게임 가드 동의창 WebPage로 처리 - STRMSG_090831_0001는 웹페이지로 처리
//#define STRMSG_090831_0001					"AirRivals is now protected from cheaters with a hackshield.\r\nPlease install it to help us to make AirRivals even safer.\r\nYou can only continue gameplay once you have installed the hackshield.\r\nPlease read the privacy policy< http://agb.gameforge.de/mmog/index.php?lang=en&art=datenschutz_mmog&special=airrivals&&f_text=b1daf2&f_text_hover=ffffff&f_text_h=061229&f_text_hr=061229&f_text_hrbg=061229&f_text_hrborder=9EBDE4&f_text_font=arial%2C+arial%2C+arial%2C+sans-serif&f_bg=000000 > to find out more about the hackshield's function."
#define STRMSG_090831_0002					"Install hackshield"
#define STRMSG_090831_0003					"cancel"

///////////////////////////////////////////////////////////////////////////////
// 2009-09-02 by cmkwon, Gameforge4D 게임 가드 동의창 WebPage로 처리 - 
#define STRMSG_090902_0001					"https://support.oldschoolrivals.com"

///////////////////////////////////////////////////////////////////////////////
// 2009-10-16 by cmkwon, 지원 해상도 추가(1680x1050,1920x1080,1920x1200) - 
#define STRMSG_WINDOW_DEGREE_1680x1050_LOW			"1680x1050 L"
#define STRMSG_WINDOW_DEGREE_1680x1050_MEDIUM		"1680x1050 M"
#define STRMSG_WINDOW_DEGREE_1680x1050_HIGH			"1680x1050 H"
#define STRMSG_WINDOW_DEGREE_1920x1080_LOW			"1920x1080 L"
#define STRMSG_WINDOW_DEGREE_1920x1080_MEDIUM		"1920x1080 M"
#define STRMSG_WINDOW_DEGREE_1920x1080_HIGH			"1920x1080 H"
#define STRMSG_WINDOW_DEGREE_1920x1200_LOW			"1920x1200 L"
#define STRMSG_WINDOW_DEGREE_1920x1200_MEDIUM		"1920x1200 M"
#define STRMSG_WINDOW_DEGREE_1920x1200_HIGH			"1920x1200 H"

///////////////////////////////////////////////////////////////////////////////
// 2011-01-26 by hskim, 인증 서버의 접속 허용 상황
#define STRMSG_AUTHENTICATION_ACCEPT_COMMENT_NOT_REGISTER			"Not Registered Server (It can be Illegal Private Server.)"
#define STRMSG_AUTHENTICATION_ACCEPT_COMMENT_DB_ERROR				"Error occurs during linkage with DB"
#define STRMSG_AUTHENTICATION_ACCEPT_COMMENT_OK						"Authentication succeed"
#define STRMSG_AUTHENTICATION_ACCEPT_COMMENT_BLOCKED				"Registered Server IP But proceed Authentication Denied"
#define STRMSG_AUTHENTICATION_ACCEPT_COMMENT_SHUTDOWN				"Windows Shut Down command is sent to the server."

#endif // end_#ifndef _STRING_DEFINE_SERVER_H_
