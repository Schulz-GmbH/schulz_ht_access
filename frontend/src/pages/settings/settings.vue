<template>
	<div class="p-2 space-y-6" id="wlan">
		<!-- Header -->
		<div class="space-y-1 text-xs">
			<div class="flex items-center justify-center mb-2">
				<div class="flex justify-center items-center w-12 h-12 rounded-xl bg-slate-800/10 p-3 text-slate-700">
					<i class="text-xl fas fa-wifi text-slate-700"></i>
				</div>
			</div>
			<h2 class="text-2xl font-semibold text-center">
				{{ $t('pages.settings.header.wlan.title') }}
			</h2>
			<p class="text-center text-slate-500">
				{{ $t('pages.settings.header.wlan.description') }}
			</p>
		</div>

		<!-- Toggle WLAN On/Off -->
		<div class="bg-white rounded-2xl p-4 shadow flex flex-col items-center justify-between space-y-2 text-xs">
			<div class="flex w-full items-center justify-between">
				<div class="font-medium text-sm">
					{{ $t('pages.settings.toggle.wlan') }}
				</div>
				<label class="relative inline-block w-10 h-5">
					<input type="checkbox" class="sr-only peer" @change="onToggleCheckbox('wlan', $event)"
						:checked="systemStore.wlan.connection.status" />
					<span class="block w-full h-full rounded-full bg-slate-800/10 peer-checked:bg-blue-500 transition-all duration-200
                 before:absolute before:top-0.5 before:left-0.5 before:w-4 before:h-4 before:rounded-full before:bg-white
                 before:transition-transform peer-checked:before:translate-x-5"></span>
				</label>
			</div>
			<div v-if="currentSSID"
				class="w-full text-slate-600 flex items-center justify-between hover:bg-slate-50 pt-3 cursor-pointer border-t border-gray-200">
				<div class="h-4 w-4 flex-none flex justify-center items-center space-x-3">
					<i class="fa-solid fa-check text-blue-400"></i>
				</div>
				<span class="flex-1 truncate mx-4">{{ currentSSID }}</span>
				<div class="flex items-center space-x-3">
					<i class="fas fa-lock text-slate-400"></i>
					<i class="fas fa-wifi text-slate-600"></i>
					<i class="fas fa-info-circle text-blue-500"></i>
				</div>
			</div>
		</div>

		<!-- Meine Netzwerke -->
		<div v-if="systemStore.wlan.connection.status" class="text-xs space-y-2">
			<div class="font-semibold text-slate-500 uppercase px-2">
				{{ $t('pages.settings.networks.myNetworks') }}
			</div>
			<ul class="bg-white rounded-2xl shadow divide-y divide-slate-200">
				<li v-for="net in savedNetworks" :key="net.ssid"
					class="flex items-center justify-between px-4 py-3 hover:bg-slate-50 cursor-pointer"
					@click="onClickSaved(net.ssid)">
					<div class="h-4 w-4 flex-none flex justify-center items-center space-x-3">
						<i class="fa-solid text-blue-400"></i>
					</div>
					<span class="flex-1 truncate mx-4">{{ net.ssid }}</span>
					<div class="flex flex-none items-center space-x-3">
						<i class="fas fa-lock text-slate-400"></i>
						<i class="fas fa-wifi text-slate-600"></i>
						<i class="fas fa-info-circle text-blue-500"></i>
					</div>
				</li>
				<li v-if="savedNetworks.length === 0" class="px-4 py-3 text-center text-slate-400">
					{{ $t('pages.settings.networks.noSaved') }}
				</li>
			</ul>
		</div>
		<!-- Andere Netzwerke -->
		<div v-if="systemStore.wlan.connection.status" class="text-xs space-y-2">
			<div class="font-semibold text-slate-500 uppercase px-2">
				{{ $t('pages.settings.networks.otherNetworks') }}
			</div>
			<ul class="bg-white rounded-2xl shadow divide-y divide-slate-200">
				<li v-if="loadingScannedNetworks" class="px-4 py-3 text-center text-slate-500">
					<i class="fas fa-spinner fa-spin mr-2"></i> Scan Networks…
				</li>
				<li v-for="net in otherNetworks" :key="net.ssid"
					class="flex items-center justify-between px-4 py-3 hover:bg-slate-50 cursor-pointer"
					@click="openPasswordModal(net.ssid)">
					<div class="h-4 w-4 flex-none flex justify-center items-center space-x-3">
						<i class="fa-solid text-blue-400"></i>
					</div>
					<span class="flex-1 truncate mx-4">{{ net.ssid }}</span>
					<div class="flex items-center space-x-3">
						<i v-if="net.security" class="fas fa-lock text-slate-400" aria-label="Gesichertes Netzwerk"></i>
						<i class="fas fa-wifi text-slate-600"></i>
						<i class="fas fa-info-circle text-blue-500"></i>
					</div>
				</li>
				<li v-if="!loadingScannedNetworks && otherNetworks.length === 0"
					class="px-4 py-3 text-center text-slate-400">
					{{ $t('pages.settings.networks.noOther') }}
				</li>
			</ul>
		</div>
	</div>


	<!-- Sprache umschalten -->
	<div class="p-2 space-y-6" id="logging">
		<!-- Header -->
		<div class="space-y-1 text-xs">
			<div class="flex items-center justify-center mb-2">
				<div class="flex justify-center items-center w-12 h-12 rounded-xl bg-slate-800/10 p-3 text-slate-700">
					<i class="text-xl fas fa-language text-slate-700"></i>
				</div>
			</div>
			<h2 class="text-2xl font-semibold text-center">
				{{ $t('language.label') }}
			</h2>
			<p class="text-center text-slate-500">
				{{ $t('language.description') }}
			</p>
		</div>
		<div class="bg-white rounded-2xl shadow p-4 flex items-center justify-between text-xs">
			<div class="font-medium text-sm">
				{{ $t('language.label') }}
			</div>
			<label class="relative inline-block w-26 h-5">
				<select id="languageSelect" v-model="language" @change="changeLanguage"
					class=" px-4 w-full h-full rounded-full bg-slate-800/10 text-center">
					<option v-for="opt in languageOptions" :key="opt.value" :value="opt.value"
						class="bg-white text-slate-700">
						{{ $t(opt.labelKey) }}
					</option>
				</select>
			</label>
		</div>
	</div>


	<div class="p-2 space-y-6" id="logging">
		<!-- Header -->
		<div class="space-y-1 text-xs">
			<div class="flex items-center justify-center mb-2">
				<div class="flex justify-center items-center w-12 h-12 rounded-xl bg-slate-800/10 p-3 text-slate-700">
					<i class="text-xl fas fa-file-lines text-slate-700"></i>
				</div>
			</div>
			<h2 class="text-2xl font-semibold text-center">
				{{ $t('pages.settings.header.logging.title') }}
			</h2>
			<p class="text-center text-slate-500">
				{{ $t('pages.settings.header.logging.description') }}
			</p>
		</div>

		<!-- Toggle WLAN On/Off -->
		<div class="bg-white rounded-2xl shadow p-4 flex items-center justify-between text-xs">
			<div class="font-medium text-sm">
				{{ $t('pages.settings.toggle.logging') }}
			</div>
			<label class="relative inline-block w-10 h-5">
				<input type="checkbox" class="sr-only peer" @change="onToggleCheckbox('logging', $event)"
					:checked="systemStore.logging.state" />
				<span class="block w-full h-full rounded-full bg-slate-800/10 peer-checked:bg-blue-500 transition-all duration-200
                 before:absolute before:top-0.5 before:left-0.5 before:w-4 before:h-4 before:rounded-full before:bg-white
                 before:transition-transform peer-checked:before:translate-x-5"></span>
			</label>
		</div>
	</div>

	<!-- Passwort-Modal -->
	<Modal v-model="passwordModalVisible" @confirm="confirmPassword">
		<template #header>
			{{ $t('pages.settings.modal.passwordHeader', { ssid: selectedSSID }) }}
		</template>
		<template #default>
			<input v-model="password" type="password" :placeholder="$t('pages.settings.modal.passwordPlaceholder')"
				class="w-full p-2 rounded bg-slate-800/10 text-white" />
		</template>
		<template #footer>
			<button class="btn" @click="passwordModalVisible = false">{{ $t('buttons.cancel') }}</button>
			<button class="btn btn-primary" @click="confirmPassword">{{ $t('buttons.connect') }}</button>
		</template>
	</Modal>
</template>

<script lang="ts">
export { default } from './settings'

</script>

<style src="./settings.scss" scoped></style>
