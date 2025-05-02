<template>
	<div class="flex flex-wrap flex-1 -mx-3 xl:flex-col" id="terminal">
		<!-- <div class="w-full max-w-full px-3 lg:flex-0 shrink-0 lg:w-1/2"> -->
		<Widget title="Terminal" icon="fas fa-expand" widgetWidth="xl:w-full" contentDirection="col" contentGap="6"
			iconBackground="bg-transparent" iconColor="text-slate-500" leftWidth="w-1/2 xl:w-2/3">
			<template #body>
				<span v-if="systemStore.wlan.status">Aktiv&nbsp;</span>
				<span v-else>Gerät&nbsp;</span>
				<span class="text-sm font-bold"
					:class="systemStore.serial.available && systemStore.serial.connected ? 'text-lime-500' : 'text-red-500'">
					{{ systemStore.serial.available && systemStore.serial.connected ? 'verbunden' : 'getrennt' }}
				</span>
			</template>
			<template #right>
				<div class="flex flex-none w-1/2 xl:w-1/3 justify-end items-center px-3">
					<div v-if="systemStore.serial.connected" class="flex justify-end pr-3 basis-1/3">
						<div class="flex items-center justify-center w-12 h-12 text-center rounded-lg bg-transparent"
							@click="toggleFullscreen">
							<div class="flex items-center justify-center text-lg">
								<i class="fas fa-expand"></i>
							</div>
						</div>
					</div>
					<div class="flex flex-col justify-center items-center w-full h-8 text-center rounded-lg max-w-24">
						<button class="h-full w-full cursor-pointer text-white rounded-lg mx-4"
							:class="systemStore.serial.available && systemStore.serial.connected ? 'bg-red-500' : 'bg-green-500'"
							@click="toggleConnection">
							{{ systemStore.serial.available && systemStore.serial.connected ? 'Trennen' :
								'Verbinden' }}
						</button>
					</div>
				</div>
			</template>
			<template #content>
				<Transition name="fancy-expand">
					<div v-if="systemStore.serial.connected" class="flex flex-col w-full h-full">
						<!-- Terminal-Feld -->
						<div class="terminal-content w-full flex flex-1 flex-row flex-nowrap gap-6">
							<div class="w-full h-full bg-gray-50 border border-solid border-gray-300 rounded-lg">
								<div ref="scrollRef" class="terminal-scroll overflow-y-auto h-full">
									<textarea ref="terminalTextarea" name="terminalOutput" id="terminalOutput" readonly
										class="w-full h-full bg-transparent border-none outline-none no-scrollbar ">{{ terminalOutput
										}}</textarea>
								</div>
							</div>
						</div>

						<!-- Eingabezeile -->
						<div class="keyboard w-full flex flex-none flex-row flex-nowrap gap-6 mt-4">
							<div class="flex flex-col w-4/6 max-w-full">
								<label class="mt-2 mb-2 ml-1 font-bold text-xs" for="ProductName">Befehl</label>
								<input type="text" ref="command" name="command" placeholder="Start"
									@keyup.enter="sendData"
									class="focus:shadow-soft-primary-outline text-sm leading-5.6 ease-soft block appearance-none rounded-lg border border-solid border-gray-300 bg-white bg-clip-padding px-3 py-2 font-normal text-gray-700 outline-none transition-all placeholder:text-gray-500 focus:border-blue-300 focus:outline-none" />
							</div>
							<div class="flex flex-col w-2/6 max-w-full justify-end items-end">
								<button @click="sendData"
									class="focus:shadow-soft-primary-outline text-sm text-center leading-5.6 ease-soft block w-full appearance-none rounded-lg border border-solid border-gray-300 bg-white bg-clip-padding px-3 py-2 font-normal text-gray-700 outline-none transition-all placeholder:text-gray-500 focus:border-blue-300 focus:outline-none">
									Senden
								</button>
							</div>
						</div>
					</div>
				</Transition>
			</template>
		</Widget>
		<!-- </div> -->
	</div>

	<div v-if="showModal" class="fixed inset-0 p-3 flex items-center justify-center bg-gray-900/75 z-[1000]">
		<div class="bg-white rounded-lg p-6 shadow-lg w-96">
			<h3 class="text-lg font-semibold mb-4">Baudrate auswählen</h3>
			<select v-model="selectedBaudRate"
				class="w-full p-2 border rounded-md text-gray-800 focus:outline-none focus:ring focus:border-blue-300">
				<option v-for="rate in baudRates" :key="rate" :value="rate">
					{{ rate }} Baud
				</option>
			</select>
			<div class="flex justify-end mt-4">
				<button class="px-4 py-2 bg-gray-500 text-white rounded-md mr-2" @click="closeBaudrateModal">
					Abbrechen
				</button>
				<button class="px-4 py-2 bg-green-500 text-white rounded-md" @click="confirmConnection">
					Verbinden
				</button>
			</div>
		</div>
	</div>
</template>

<script lang="ts">
export { default } from './terminal'
</script>

<style src="./terminal.scss"></style>

<style lang="scss" scoped>
#terminal.fullscreen {
	position: fixed !important;
	top: 0 !important;
	left: 0 !important;
	width: 100vw !important;
	height: 100vh !important;
	z-index: 9999 !important;
	padding: 1rem;
	margin: 0;
	background: #000;
}

#terminal #widget {
	padding: 0;
	margin: 0;
}

.fancy-expand-enter-active,
.fancy-expand-leave-active {
	transition:
		opacity 1s ease,
		transform 1s ease,
		max-height 1s ease;
	overflow: hidden;
}

.fancy-expand-enter-from {
	opacity: 0;
	transform: translateY(1rem);
	max-height: 0;
}

.fancy-expand-enter-to {
	opacity: 1;
	transform: translateY(0);
	max-height: 1000px; // Passe ggf. an
}

.fancy-expand-leave-from {
	opacity: 1;
	transform: translateY(0);
	max-height: 1000px;
}

.fancy-expand-leave-to {
	opacity: 0;
	transform: translateY(1rem);
	max-height: 0;
}

textarea.no-scrollbar {
	width: 100%;
	padding: 0.5rem;
	resize: none;
	scrollbar-width: none;
	-ms-overflow-style: none;
}

/* 2) Scrollbar ausblenden */
/* Firefox */
textarea.no-scrollbar {
	scrollbar-width: none;
}

/* IE 10+ */
textarea.no-scrollbar {
	-ms-overflow-style: none;
}

/* WebKit (Chrome, Safari) */
textarea.no-scrollbar::-webkit-scrollbar {
	display: none;
}
</style>
