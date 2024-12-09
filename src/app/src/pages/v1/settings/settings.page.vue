<template>
	<section class="p-3 grid grid-cols-[49%_49%] gap-x-6 gap-y-6 flex-wrap :p-[12px_0] sm:grid-cols-[100%] sm:gap-y-3">
		<!-- WLAN Setting Widget -->
		<form @submit.prevent="submitWiFiConfig"
			class="widget flex flex-col t p-4 bg-white border-none rounded-sm shadow-md">
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
					<input type="text" id="ssid" v-model="wifiData.ssid"
						:placeholder="existingWiFiData?.ssid || 'SSID eingeben'" required
						class="p-2 text-base border border-gray-300 focus:outline-none focus:ring-2 focus:ring-blue-500" />
				</div>
				<div class="form-group flex flex-col">
					<label for="password" class="font-semibold">Passwort</label>
					<input type="password" id="password" v-model="wifiData.password"
						:placeholder="existingWiFiData?.ssid ? '******' : 'Passwort eingeben'" required
						class="p-2 text-base border border-gray-300 focus:outline-none focus:ring-2 focus:ring-blue-500" />
				</div>
			</div>

			<!-- Footer -->
			<div v-if="message"
				class="widget-footer mt-4 flex items-center justify-between border-t border-gray-300 pt-2">
				<span class="text-xs text-gray-500" :class="{ 'text-red-700': isError }">
					{{ message }}
				</span>
			</div>
		</form>

		<wlan-widget />
	</section>
</template>

<script src="./settings.page.ts"></script>
