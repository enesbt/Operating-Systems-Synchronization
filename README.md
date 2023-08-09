# Operating-Systems-Synchronization
## Üretici-Tüketici adında iki adet proses bir bellek bölgesini paylaşımlı olarak kullanmaktadırlar. Üretici proses veri yazmakta ve Tüketici proses okuma işlemi yapmaktadır.
1.	Her iki proses (Üretici-Tüketici) yavru proses oluşturacak ve belleğe yazma-okuma işlemlerini bu yavru proses üzerinden yaptırılmaktadırlar.
2.	Üretici proses yavru prosese yazacağı veriyi pipe/tünel mekanizması ile göndermekte aynı şekilde tüketicide ise yavru okuduğu veriyi ebeveyn prosese pipe/tünel mekanizması ile göndermektedir. 
3.	Üretici proses 10 adet iş parçacığı/thread oluşturup 1-10 arası sayıları bir saniye aralıkla ve sırasıyla paylaşılmış alana yazmakta (thread1 “1” yazacak, thread2 “2” yazacak …), aynı şekilde Tüketici proses 10 adet iş parçacığı/thread oluşturup 1-10 arası sayıları bir saniye aralıkla ve sırasıyla okumaktadır.
4.	Üretici prosesteki iş parçacıkları sayıları yazarken ortak global “sayac” değişkenini arttırarak yazacak ve üretici-tüketici proseslerinde herhangi bir veri tutarsızlığı oluşmaması için ortak kullanılan değişkenler için kritik bölge tanımlanarak işlemler gerçekleştirilecektir.
5.	Her iki proses aynı anda çalışacak ve üretici proses 1-10 arası sayıları yazdıktan sonra tüketici proses 1-10 arası sayıları sırasıyla okuyacak ve bu işlem bittiği zaman sürekli olarak tekrarlanacak.
