<?php
	extension_loaded('openssl') or die('php need openssl extended support');
	
	class Crypt
	{
		private $data;  //The data that needs to be encrypted
		private $private_key;
		private $public_key;
		private $des_key = '';
		private $encrypt_des_key = '';
		private $decrypt_des_key = '';
	
		function __construct(){
			$web_client_file_name = 'D:\Cache\1697500213\FileRecv\CreateEncryptedString\key\web-client.p12';
			if(file_exists($web_client_file_name)){
				$password = 'openssl';
					
				$certificate = array();
				
				$worked = openssl_pkcs12_read(file_get_contents($web_client_file_name), $certificate, $password);
				if($worked){
					//echo print_r($certificate, true);
					//echo print_r($certificate['pkey'], true);
					//echo print_r($certificate['cert'], true);
					$private_key_content = $certificate['pkey'];	//get private key
					//$this->passphrase = null;
					$public_key_content = $certificate['cert'];	//get public key
				}
				else
					echo openssl_error_string();
				
					//get the private key
					$this->private_key = openssl_pkey_get_private($private_key_content);
					//get the public key
					$this->public_key = openssl_pkey_get_public($public_key_content);
					//generate the des key
					$this->generate_des_key();
			}
			else
				echo "Encrypt file is not exists!\n";
			
		}
		
		public function get_encrypt_data($input_file = ''){
			if(!file_exists($input_file))
				$input_file='D:\Cache\1697500213\FileRecv\CreateEncryptedString\out\production\CreateEncryptedString\input.txt';
			//get data that needs to be encrypted
			$this->data = file_get_contents($input_file);
		}
	
		public function signature(){
			// 摘要及签名算法
			$digestAlgo = 'MD5';	//单项散列算法
			$algo = OPENSSL_ALGO_MD5;	// 或者 $algo = OPENSSL_ALGO_SHA1;
				
			// 生成摘要
			$digest = openssl_digest($this->data, $digestAlgo);
	
			// 私钥签名
			$signature = '';
			openssl_sign($digest, $signature, $this->private_key, $algo);
			$signature = base64_encode($signature);
	
			var_dump($signature);
			return $signature;
		}
	
		public function verify($signature){
			// 摘要及签名的算法，同上面一致
			$digestAlgo = 'MD5';
			$algo = OPENSSL_ALGO_MD5;
	
			// 生成摘要
			$digest = openssl_digest($this->data, $digestAlgo);
	
			// 验签
			$verify = openssl_verify($digest, base64_decode($signature), $this->public_key, $algo);
			var_dump($verify); // int(1)表示验签成功
		}
	
		public function RsaEncrypt(){
			// 使用公钥进行 des_key 的加密
			$encryptedData = '';
			openssl_public_encrypt($this->des_key, $encryptedData, $this->public_key);
				
			$this->encrypt_des_key = base64_encode($encryptedData);
			var_dump($this->encrypt_des_key);
		}
	
		public function RsaDecrypt(){
			// 使用私钥进行 des_key的解密 
			openssl_private_decrypt(base64_decode($this->encrypt_des_key), $this->decrypt_des_key, $this->private_key);
	
			var_dump($this->decrypt_des_key); // 应该跟 $des_key 一样
		}
	
		public function DesEncrypt()
		{
			$iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_ECB);
			$iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
				
			//为文本数据加密
			$encrypttext =base64_encode(mcrypt_encrypt(MCRYPT_RIJNDAEL_128, $this->des_key, $this->data, MCRYPT_MODE_ECB, $iv));
			//echo $encrypttext;	//加密后的内容
				
			return $encrypttext;
		}
	
		public function DesDecrypt($encrypttext)
		{
			$iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_ECB);
			$iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
	
	
			//为文本数据解密
			return mcrypt_decrypt(MCRYPT_RIJNDAEL_128,$this->des_key,base64_decode($encrypttext),MCRYPT_MODE_ECB,$iv);
		}
	
		//生成随机的des密钥，默认为16位
		private function generate_des_key($length=16)
		{
			for($i = 0; $i < $length; $i++)
				$this->des_key .=mt_rand(0,9);
		}
	}
?>