<template>
	<div class="flex flex-wrap -mx-3 xl:flex-col transition-all duration-300" ref="keyboardEl">
		<div class="w-full max-w-full px-3 xl:w-full p-4 bg-gray-50">
			<!-- Input + Toggle + Enter -->
			<div class="flex mb-4">
				<input v-model="command" ref="inputEl" :readonly="isMobile" @focus="showKeyboard"
					class="flex-1 px-2 py-1 border border-gray-300 rounded-l-lg bg-white text-lg"
					:placeholder="$t('components.mobileKeyboard.placeholder')" @mousedown.prevent="showKeyboard"
					@touchstart.prevent="showKeyboard" />
				<button @click.stop="keyboardVisible == false ? showKeyboard() : switchKeyboard()"
					class="px-4 bg-gray-200 border-t border-b border-r border-gray-300 rounded-r-lg hover:bg-gray-300 flex items-center space-x-1">
					<i class="fas fa-keyboard"></i>
				</button>
				<button @click.stop="sendCommand()"
					class="ml-2 px-4 bg-blue-600 text-white rounded-lg hover:bg-blue-700">
					⏎
				</button>
			</div>

			<transition name="slide-up">
				<div v-show="keyboardVisible && isMobile" class="keyboard-container">
					<!-- Custom Keypad -->
					<div v-if="customActive" class="space-y-2">
						<div v-for="(row, ri) in customKeys" :key="ri" class="grid gap-2"
							:style="{ gridTemplateColumns: `repeat(${row.length}, minmax(0, 1fr))` }">
							<button v-for="key in row" :key="key" @click.stop="onPress(key)"
								class="py-2 bg-white rounded-lg shadow text-center hover:bg-gray-100 select-none touch-manipulation">
								<span v-html="renderKeyLabel(key)"></span>
							</button>
						</div>
					</div>

					<!-- Standard Keypad -->
					<div v-else class="space-y-2">
						<!-- Alpha -->
						<template v-if="stdMode === 'alpha'">
							<div v-for="(row, ri) in alphaRows" :key="ri" class="grid gap-2"
								:style="{ gridTemplateColumns: `repeat(${row.length}, minmax(0, 1fr))` }">
								<button v-for="key in row" :key="key" @click.stop="onPress(key)"
									class="py-2 bg-white rounded-lg shadow text-center hover:bg-gray-100 select-none touch-manipulation">
									<span v-html="renderKeyLabel(key)"></span>
								</button>
							</div>
						</template>

						<!-- Numeric -->
						<template v-else-if="stdMode === 'numeric'">
							<div v-for="(row, ri) in numericRows" :key="ri" class="grid gap-2"
								:style="{ gridTemplateColumns: `repeat(${row.length}, minmax(0, 1fr))` }">
								<button v-for="key in row" :key="key" @click.stop="onPress(key)"
									class="py-2 bg-white rounded-lg shadow text-center hover:bg-gray-100 select-none touch-manipulation">
									<span v-html="renderKeyLabel(key)"></span>
								</button>
							</div>
						</template>

						<!-- Special -->
						<template v-else>
							<div v-for="(row, ri) in specialRows" :key="ri" class="grid gap-2"
								:style="{ gridTemplateColumns: `repeat(${row.length}, minmax(0, 1fr))` }">
								<button v-for="key in row" :key="key" @click.stop="onPress(key)"
									class="py-2 bg-white rounded-lg shadow text-center hover:bg-gray-100 select-none touch-manipulation">
									<span v-html="renderKeyLabel(key)"></span>
								</button>
							</div>
						</template>

						<!-- Bottom Row -->
						<div class="flex justify-center space-x-1 mt-2">
							<button v-for="key in bottomRow" :key="key" @click.stop="onBottomPress(key)"
								class="flex-1 py-3 rounded-lg shadow text-center hover:bg-gray-100 select-none">
								<span v-html="renderKeyLabel(key)"></span>
							</button>
						</div>
					</div>
				</div>
			</transition>
		</div>
	</div>
</template>

<script lang="ts">
export { default } from './mobile-keyboard'
</script>

<style src="./mobile-keyboard.scss"></style>

<style lang="scss" scoped></style>
