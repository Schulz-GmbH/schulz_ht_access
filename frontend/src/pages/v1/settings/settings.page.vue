<template>

	<div
		class="widget col-span-2 sm:col-span-1 flex flex-col items-start p-4 bg-white border-none rounded-sm shadow-md cursor-text">

		<form class="w-full">
			<!-- <form class="widget flex flex-col t p-4 bg-white border-none rounded-sm shadow-md row-span-5 col-span-3"> -->
			<!-- Header -->
			<div class="widget-header flex items-center justify-between mb-4">
				<h3 class="text-lg font-semibold text-gray-800">WLAN Einstellungen</h3>
				<button class="text-gray-500 hover:text-gray-800 focus:outline-none" type="submit">
					<i class="fas fa-save"></i>
				</button>
			</div>

			<!-- Body -->
			<div class="widget-body flex-grow text-gray-600 space-y-4">
				<div class="form-group flex flex-col space-y-2">
					<label for="ssid" class="font-semibold">SSID</label>
					<input type="text" id="ssid" required v-model="wifiData.ssid"
						:placeholder="wifiData?.ssid || 'SSID eingeben'"
						class="p-2 text-base border border-gray-300 focus:outline-none focus:ring-2 focus:ring-blue-500" />
				</div>
				<div class="form-group flex flex-col">
					<label for="password" class="font-semibold">Passwort</label>
					<input type="password" id="password" required v-model="wifiData.password"
						:placeholder="wifiData?.ssid ? '******' : 'Passwort eingeben'"
						class="p-2 text-base border border-gray-300 focus:outline-none focus:ring-2 focus:ring-blue-500" />
				</div>
			</div>

			<!-- Footer -->
			<div class="widget-footer mt-4 flex items-center justify-between border-t border-gray-300 pt-2 -mx-4">
				<span v-if="error" class="text-xs text-gray-500 mx-4" :class="{ 'text-red-700': error }">
					{{ error.message }}
				</span>
			</div>
		</form>
	</div>
	<div
		class="widget col-span-2 sm:col-span-1 flex flex-col items-start p-4 bg-white border-none rounded-sm shadow-md cursor-text">


		<form class="w-full">
			<div class="widget-header flex items-center justify-between mb-4">
				<h3 class="text-lg font-semibold text-gray-800">Update Server</h3>
				<button class="text-gray-500 hover:text-gray-800 focus:outline-none" type="submit">
					<i class="fas fa-save"></i>
				</button>
			</div>

			<div class="widget-body flex-grow text-gray-600 space-y-4">
				<div class="form-group flex flex-col space-y-2">
					<label for="ip-address" class="font-semibold">IP-Adresse</label>
					<input type="text" id="ip-address" v-model="apiData.ip" :placeholder="apiData?.ip || '80.168.5.151'"
						required @input="validateIPAddress"
						class="p-2 text-base border border-gray-300 focus:outline-none focus:ring-2 focus:ring-blue-500" />
				</div>
				<div class="form-group flex flex-col">
					<label for="port" class="font-semibold">Port</label>
					<input type="number" max="65535" min="1" id="port" v-model="apiData.port"
						:placeholder="apiData?.port ? '******' : '9000'" required
						class="p-2 text-base border border-gray-300 focus:outline-none focus:ring-2 focus:ring-blue-500" />
				</div>
			</div>

			<div class="widget-footer mt-4 flex items-center justify-between border-t border-gray-300 pt-2 -mx-4">
				<span v-if="error" class="text-xs text-gray-500 mx-4" :class="{ 'text-red-700': error }">
					{{ error.message }}
				</span>
			</div>
		</form>
	</div>
</template>

<script src="./settings.page.ts"></script>
