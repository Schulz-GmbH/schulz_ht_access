<template>
	<div class="h-full p-3 flex flex-col bg-gray-900 text-white" :style="{ height: sectionHeight }">
		<!-- Terminal Output -->
		<div ref="terminalOutput" class="debug flex-1 p-4 font-mono text-sm leading-6 overflow-y-auto">
			<div v-for="(line, index) in terminalLines" :key="index">
				<span>{{ line }}</span>
			</div>
		</div>

		<!-- Terminal Input -->
		<div
			class="flex sm:items-start items-center p-4 sm:flex-col sm:space-y-4 flex-row bg-gray-800 border-t border-gray-700">
			<!-- User Input -->
			<form @submit.prevent="handleCommand" class="flex flex-1 items-center">
				<span class="text-green-500 mr-2">$</span>
				<input v-model="userInput" type="text"
					class="flex-1 bg-transparent border-none outline-none text-white placeholder-gray-500"
					placeholder="Eingabe hier..." />
			</form>

			<!-- Dropdown und Button -->
			<div class="flex items-center space-x-4 ml-4">
				<!-- Dropdown für Baudrate -->
				<select v-if="!isConnected" v-model="selectedBaudRate"
					class="sm:w-full w-48 bg-gray-700 text-white px-2 py-1 rounded">
					<option v-for="rate in baudRates" :key="rate" :value="rate">
						{{ rate }} Baud
					</option>
				</select>

				<!-- Button für Verbindung -->
				<button @click="toggleConnection" class="bg-green-500 text-white px-4 py-1 rounded">
					{{ isConnected ? "Verbindung trennen" : "Verbinden" }}
				</button>
			</div>
		</div>
	</div>
</template>

<script src="./console.page.ts"></script>

<style scoped>
/* Optional: Scrollbar-Styling */
.debug::-webkit-scrollbar {
	width: 8px;
}

.debug::-webkit-scrollbar-thumb {
	background: #4a5568;
	border-radius: 4px;
}
</style>
